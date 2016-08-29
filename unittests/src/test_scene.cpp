#include "gtest/gtest.h"

#include <Scene.hpp>
#include <Camera.hpp>

using namespace kepler;

static void expectAllChildrenInScene(NodeRef& node, SceneRef& scene) {
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto& child = node->getChildAt(i);
        EXPECT_EQ(child->getScene(), scene);
        EXPECT_EQ(child->getParent(), node);
        expectAllChildrenInScene(child, scene);
    }
}

TEST(scene, initial_scene) {
    auto scene = Scene::create();
    EXPECT_EQ(scene->getChildCount(), 0);
}

TEST(scene, create_child) {
    auto scene = Scene::create();
    auto n1 = scene->createChild();
    EXPECT_EQ(scene->getChildCount(), 1);
    EXPECT_EQ(n1->getScene(), scene);
    EXPECT_EQ(n1->getParent(), nullptr);
    EXPECT_EQ(n1->getRoot(), n1);

    const std::string nodeName("Node Name");
    auto n2 = scene->createChild(nodeName);
    EXPECT_EQ(scene->getChildCount(), 2);
    EXPECT_EQ(n2->getName(), nodeName);
    EXPECT_EQ(n1->getScene(), scene);
    EXPECT_EQ(n2->getParent(), nullptr);
    EXPECT_EQ(n2->getRoot(), n2);
}

TEST(scene, add_child) {
    auto scene = Scene::create();

    auto root = Node::create("root");
    root->createChildren({ "A1", "A2" });
    root->getChildAt(0)->createChildren({ "B1", "B2" });
    (*root)[1]->createChildren({ "C1", "C2", "C3" });

    scene->addNode(root);
    EXPECT_EQ(root->getScene(), scene);
    expectAllChildrenInScene(root, scene);
}

TEST(scene, add_child_from_other_scene) {
    // Test case for bug in Scene::addNode
    auto s1 = Scene::create();
    auto s2 = Scene::create();
    auto node = s1->createChild("node");
    s2->addNode(node);
    EXPECT_EQ(s1->getChildCount(), 0);
    EXPECT_EQ(s2->getChildCount(), 1);
    EXPECT_EQ(s2, node->getScene());
}

TEST(scene, find_first) {
    auto scene = Scene::create();
    auto root = Node::create("root");
    scene->addNode(root);
    root->createChildren({ "A1", "A2" });
    auto a1 = root->getChildAt(0);
    auto a2 = root->getChildAt(1);
    a1->createChildren({ "B1", "B2" });
    a2->createChildren({ "C1", "C2", "C3", "C4", "C5" });

    // found
    EXPECT_EQ(scene->findFirstNodeByName("root"), root);
    EXPECT_EQ(scene->findFirstNodeByName("A1"), a1);
    EXPECT_EQ(scene->findFirstNodeByName("A2"), a2);

    EXPECT_NE(scene->findFirstNodeByName("B1"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("B2"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("C1"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("C2"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("C3"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("C4"), nullptr);
    EXPECT_NE(scene->findFirstNodeByName("C5"), nullptr);

    // not found
    EXPECT_EQ(scene->findFirstNodeByName("UNKNOWN"), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName(""), nullptr);

    // non-recursive. Should not find
    EXPECT_EQ(scene->findFirstNodeByName("A1", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("A2", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("B1", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("B2", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("C1", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("C2", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("C3", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("C4", false), nullptr);
    EXPECT_EQ(scene->findFirstNodeByName("C5", false), nullptr);

    // non-recursive. Should find
    EXPECT_NE(scene->findFirstNodeByName("root", false), nullptr);

}

TEST(scene, find_first_eval) {
    auto scene = Scene::create();
    auto root = Node::create("root");
    scene->addNode(root);
    root->createChildren({ "A1", "A2" });
    auto a1 = root->getChildAt(0);
    auto a2 = root->getChildAt(1);
    a1->createChildren({ "B1", "B2" });
    a2->createChildren({ "C1", "C2", "C3", "C4", "C5" });

    auto c3 = scene->findFirstNodeByName("C3");
    auto camera = Camera::createPerspectiveFov(45.0f, 800.0f, 600.0f, 0.1f, 100.0f);
    c3->addComponent(camera);

    auto containsCamera = [](Node* node) -> bool {
        return node->getComponent<Camera>() != nullptr;
    };
    EXPECT_EQ(c3, scene->findFirstNode(containsCamera));

    auto isLeaf = [](Node* node) -> bool {
        return node->getChildCount() == 0;
    };
    EXPECT_EQ(scene->findFirstNodeByName("B1"), scene->findFirstNode(isLeaf));
}

TEST(scene, get_children) {
    auto scene = Scene::create();
    EXPECT_EQ(scene->getLastChild(), nullptr);

    auto n1 = scene->createChild("1");
    EXPECT_EQ(scene->getLastChild(), n1);
    auto n2 = scene->createChild("2");
    auto n3 = scene->createChild("3");
    auto n4 = scene->createChild("4");
    auto n5 = scene->createChild("5");
    EXPECT_EQ(scene->getLastChild(), n5);
    EXPECT_EQ(scene->getChildAt(3), n4);

    EXPECT_EQ(scene->getChildCount(), scene->getChildren().size());
    for (auto& child : scene->getChildren()) {

    }
}

TEST(scene, visit) {
    auto scene = Scene::create();
    auto root = Node::create("root");
    scene->addNode(root);
    root->createChildren({ "A1", "A2" });
    auto a1 = root->getChildAt(0);
    auto a2 = root->getChildAt(1);
    a1->createChildren({ "B1", "B2" });
    a2->createChildren({ "C1", "C2", "C3", "C4", "C5" });

    root->findFirstNodeByName("B1")->createChild("D1");

    std::vector<std::string> expectedOrder{ "root", "A1", "B1", "D1", "B2", "A2", "C1", "C2", "C3", "C4", "C5" };
    std::vector<std::string> actualOrder;
    auto orderTest = [&actualOrder](Node* node) {
        actualOrder.push_back(node->getName());
    };
    scene->visit(orderTest);
    EXPECT_EQ(expectedOrder, actualOrder);

    // check count
    unsigned int count = 0;
    auto countFunc = [&count](Node* node) {
        ++count;
    };
    scene->visit(countFunc);
    EXPECT_EQ(count, expectedOrder.size());
}

TEST(scene, move_nodes_from) {
    auto a = Scene::create();
    auto b = Scene::create();
    b->createChildren({ "A1", "A2", "A3" });
    b->getChildAt(0)->createChildren({ "B1", "B2" });
    b->getChildAt(1)->createChildren({ "C1", "C2", "C3" });
    b->getChildAt(2)->createChildren({ "D1" });
    b->getChildAt(1)->getChildAt(1)->createChildren({ "E1", "E2", "E3" });

    auto childCount = b->getChildCount();
    a->moveNodesFrom(b);
    EXPECT_EQ(b->getChildCount(), 0);
    EXPECT_EQ(a->getChildCount(), childCount);
    // check order
    EXPECT_EQ(a->getChildAt(0)->getName(), "A1");
    EXPECT_EQ(a->getChildAt(2)->getName(), "A3");

    a->visit([a](Node* node) {
        EXPECT_EQ(node->getScene(), a);
    });
}
