#pragma once
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <phys_utils.h>
#include <string>
#include <vector>

class Entity;

class Component {
protected:
  Entity *Ent_;
  bool active_;

public:
  std::string token_;
  Component(const std::string &token);
  virtual ~Component();
  virtual void Update(double delta){};
  virtual void Render(){};
  bool IsActive();
  void SetActive(bool b);
  virtual void SetParent(Entity *p);
  Entity *GetParent() const;
};

class Entity {
protected:
  bool visible_;
  std::string name_;
  bool changed_;
  glm::dvec3 scale_;
  glm::dvec3 position_;
  glm::dquat rotation_;
  glm::dmat4 transform_;
  std::vector<std::unique_ptr<Component>> components_;

public:
  phys::RGBAInt32 colour;
  Entity();
  ~Entity();

  const glm::dvec3 GetScale() const;
  const glm::dvec3 GetPosition() const;
  const glm::dvec3 GetRotationV3() const;
  const glm::dquat GetRotation() const;
  const glm::dmat4 GetTranform();
  const bool IsVisible() const;
  const std::string GetName() const;

  void SetTransform(const glm::dmat4 m4);
  void SetScale(const glm::dvec3 &v3);
  void SetPosition(const glm::dvec3 &v3);
  void SetRotation(const glm::dvec3 &v3);
  void SetRotation(const glm::dquat &q);

  void SetVisibility(const bool b);
  void SetName(std::string const &name);

  virtual void Update(const double delta);
  virtual void Render();

  void AddComponent(std::unique_ptr<Component> &c);
  void RemoveComponent(Component &c);
  const std::vector<std::unique_ptr<Component>> *GetComponents() const;
  std::vector<Component *> GetComponents(std::string const &name) const;

  template <typename T> T *const getComponent() {
    for (size_t i = 0; i < components_.size(); i++) {
      if (&typeid(*components_[i]) == &typeid(T)) {
        return static_cast<T *>(&*components_[i]);
      }
    }
    return NULL;
  }
  // Will return a T component, or anything derived from a T component.
  template <typename T> T *const getCompatibleComponent() {
    for (size_t i = 0; i < components_.size(); i++) {
      auto dd = dynamic_cast<T *>(&*components_[i]);
      if (dd) {
        return dd;
      }
    }
    return NULL;
  }
};

class cShapeRenderer : public Component {
public:
  enum SHAPES { SPHERE, BOX };
  const SHAPES shape;
  void SetColour(const phys::RGBAInt32 c);
  cShapeRenderer(SHAPES shape);
  ~cShapeRenderer();
  void Update(double delta);
  void Render();

private:
  phys::RGBAInt32 col_;
};
