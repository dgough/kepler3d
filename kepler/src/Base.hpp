#pragma once

#include <memory>

namespace kepler {

class Node;
class Scene;
class Camera;
class Bounded;
class DrawableComponent;
class Button;
class BoundingBox;

class App;
class AppDelegate;
class FirstPersonController;

template<class T>
using ref = std::shared_ptr<T>; /// ref is an alias for std::shared_ptr
}
