#include "gtest/gtest.h"

#include <Node.hpp>
#include <Camera.hpp>
#include <Mesh.hpp>
#include <MeshRenderer.hpp>

using namespace kepler;

// TODO add a test for checking that all children point to their parent

CameraRef createCamera() {
    return Camera::createPerspectiveFov(45.0f, 800.0f, 600.0f, 0.1f, 100.0f);
}

TEST(node, initial_node) {
    const std::string name("asdf");
    auto node = Node::create(name);
    EXPECT_EQ(node->getChildCount(), 0);
    EXPECT_EQ(node->hasParent(), false);
    EXPECT_EQ(node->getParent(), nullptr);
    EXPECT_EQ(node->getRoot(), node);
    EXPECT_EQ(node->getScene(), nullptr);
    EXPECT_STREQ(node->getNamePtr(), name.c_str());
    EXPECT_EQ(node->getName(), name);
}

TEST(node, node_create_and_remove) {
    std::string rootName("root");
    auto root = Node::create();
    root->setName(rootName);
    EXPECT_EQ(rootName, root->getName());
    root->createChild("child 01");
    root->createChild("child 02");
    root->createChild("child 03");
    EXPECT_EQ(root->getChildCount(), 3);
    root->getChildAt(1)->removeFromParent();
    EXPECT_EQ(root->getChildCount(), 2);
}

TEST(node, add_nullptr) {
    auto n = Node::create();
    n->addNode(NodeRef(nullptr));
    EXPECT_EQ(n->getChildCount(), 0);
}

TEST(node, get_root_node) {
    auto root = Node::create("Root");

    auto n = root;
    NodeRef mid = nullptr;
    constexpr size_t len = 10;
    for (size_t i = 0; i < len; ++i) {
        n = n->createChild();
        if (i == len / 2) {
            mid = n;
        }
    }
    mid->setName("mid");
    auto leaf = n;
    while (n != root) {
        EXPECT_EQ(n->getRoot(), root);
        n = n->getParent();
    }
    mid->removeFromParent();
    EXPECT_EQ(leaf->getRoot(), mid);

    NodeWeakRef rootWeak = root;
    root.reset();
    EXPECT_EQ(root, nullptr);
}

TEST(node, get_root_node_2) {
    NodeRef root = Node::create("root");
    NodeRef n = root->createChild("A")->createChild("B")->createChild("C");

    NodeWeakRef b = root->findFirstNodeByName("B");

    EXPECT_TRUE(n->getRoot() == root);
    EXPECT_TRUE(n->getParent() == b.lock());
    root = nullptr;
    EXPECT_TRUE(n->getRoot() == n);
    EXPECT_TRUE(n->getParent() == nullptr);
}

TEST(node, initializer_list) {
    auto root = Node::create("root");
    constexpr size_t count = 5;
    root->createChildren({ "1", "2", "3", "4", "5" });
    EXPECT_EQ(root->getChildCount(), count);
    for (size_t i = 0; i < count; ++i) {
        const std::string& name = root->getChildAt(i)->getName();
        char ch = '1' + (char)i;
        EXPECT_EQ(ch, name.at(0));
    }
}

TEST(node, find_first) {
    auto root = Node::create("root");
    root->createChildren({ "A1", "A2" });
    root->getChildAt(0)->createChildren({ "B1", "B2" });
    root->getChildAt(1)->createChildren({ "C1", "C2", "C3", "C4", "C5" });

    // found
    EXPECT_NE(root->findFirstNodeByName("C3"), nullptr);
    EXPECT_NE(root->findFirstNodeByName("C5"), nullptr);
    EXPECT_NE(root->findFirstNodeByName("A1"), nullptr);
    EXPECT_NE(root->findFirstNodeByName("B2"), nullptr);

    // not found
    EXPECT_EQ(root->findFirstNodeByName("UNKNOWN"), nullptr);
    EXPECT_EQ(root->findFirstNodeByName(""), nullptr);
    EXPECT_EQ(root->findFirstNodeByName("root"), nullptr);

    // non-recursive
    EXPECT_EQ(root->findFirstNodeByName("B2", false), nullptr);
    EXPECT_EQ(root->findFirstNodeByName("C2", false), nullptr);
    EXPECT_EQ(root->findFirstNodeByName("C3", false), nullptr);
}

TEST(node, find_first_eval) {
    auto root = Node::create("root");
    root->createChildren({ "A1", "A2" });
    root->getChildAt(0)->createChildren({ "B1", "B2" });
    root->getChildAt(1)->createChildren({ "C1", "C2", "C3", "C4", "C5" });

    auto c3 = root->findFirstNodeByName("C3");
    auto camera = createCamera();
    c3->addComponent(camera);

    auto containsCamera = [](Node* node) -> bool {
        return node->getComponent<Camera>() != nullptr;
    };
    EXPECT_EQ(c3, root->findFirstNode(containsCamera));

    auto isLeaf = [](Node* node) -> bool {
        return node->getChildCount() == 0;
    };
    EXPECT_EQ(root->findFirstNodeByName("B1"), root->findFirstNode(isLeaf));
}


TEST(node, weak_ptr_list) {

    auto a = Node::create("A");
    auto b = Node::create("B");

    std::weak_ptr<Node> weakA1 = a;
    std::weak_ptr<Node> weakA2 = a;

    std::vector<std::weak_ptr<Node>> list;

    list.push_back(weakA1);
    list.push_back(b);
    {
        list.push_back(Node::create("C"));
    }
    EXPECT_EQ(list.size(), 3);

    //list.erase(std::remove(list.begin(), list.end(), weakA2), list.end());
    //for (auto it = list.begin(); it != list.end(); ++it) {
    //    if (auto node = it->lock()) {
    //        if (node == weakA2.lock()) {
    //            list.erase(it);
    //            break;
    //        }
    //    }
    //}

    auto it = std::remove_if(list.begin(), list.end(), [weakA2](std::weak_ptr<Node> wp) {
        return (wp.lock() == weakA2.lock() || wp.expired());
    });
    list.erase(it, list.end());

    EXPECT_EQ(list.size(), 1);
}

TEST(node, contains_component) {
    NodeRef root = Node::create("root");
    auto camera = createCamera();
    
    EXPECT_FALSE(root->containsComponent(camera->getTypeName()));
    EXPECT_FALSE(root->getComponent<Camera>() != nullptr);
    root->addComponent(camera);
    EXPECT_TRUE(root->containsComponent(camera->getTypeName()));
    EXPECT_TRUE(root->getComponent<Camera>() != nullptr);
    root->removeComponent(camera.get());
    EXPECT_FALSE(root->containsComponent(camera->getTypeName()));
    EXPECT_FALSE(root->getComponent<Camera>() != nullptr);
    // TODO add other types of components
}

TEST(node, get_drawable) {
    NodeRef node = Node::create();
    EXPECT_TRUE(node->getDrawable() == nullptr);
    
    auto camera = createCamera();
    node->addComponent(camera);
    EXPECT_TRUE(node->getDrawable() == nullptr);

    auto meshRenderer = MeshRenderer::create(Mesh::create());
    node->addComponent(meshRenderer);

    EXPECT_FALSE(node->getDrawable() == nullptr);
}

class TestNodeListener : public Node::Listener {
public:
    bool _called;
    TestNodeListener() : _called(false) {}
    void transformChanged(const Node* node) override {
        _called = true;
    }
};

TEST(node, node_listener) {
    auto listener = std::make_shared<TestNodeListener>();

    auto n = Node::create();
    n->addListener(listener);
    n->translateX(5.f);
    EXPECT_TRUE(listener->_called);

    listener->_called = false;
    n->removeListener(listener.get());
    n->translateY(2.f);
    EXPECT_FALSE(listener->_called);
}

/*
std::unique_ptr<Node> root = Node::create("root");

root->createChild("child 01");
root->createChild("child 02");
root->createChild("child 03");

//root->deleteChild(1);
//root->removeChild((*root)[1]);
//root->removeChild();

auto orphan = (*root)[1]->removeParent();


std::clog << (*root)[0]->getNamePtr() << std::endl;
std::clog << (*root)[1]->getNamePtr() << std::endl;
//std::clog << (*root)[2]->getNamePtr() << std::endl;
std::clog << orphan->getNamePtr() << std::endl;
*/

/*
NodeRef root = std::make_shared<Node>("root");
{
auto node = std::make_shared<Node>("child 01");
root->addChild(node);
root->addChild(std::make_shared<Node>("child 02"));
root->addChild(std::make_shared<Node>("child 03"));
root->removeChild(node);
}
std::clog << root->getNamePtr() << std::endl;
std::clog << (*root)[0]->getNamePtr() << std::endl;
std::clog << (*root)[1]->getNamePtr() << std::endl;
NodeRef other(new Node("other"));
*/
