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
  glm::vec3 scale_;
  glm::vec3 position_;
  glm::quat rotation_;
  glm::mat4 transform_;
  std::vector<std::unique_ptr<Component>> components_;

public:
  phys::RGBAInt32 colour;
  Entity();
  ~Entity();

  const glm::vec3 GetScale() const;
  const glm::vec3 GetPosition() const;
  const glm::vec3 GetRotationV3() const;
  const glm::quat GetRotation() const;
  const glm::mat4 GetTranform();
  const bool IsVisible() const;
  const std::string GetName() const;

  void SetTransform(const glm::mat4 m4);
  void SetScale(const glm::vec3 &v3);
  void SetPosition(const glm::vec3 &v3);
  void SetRotation(const glm::vec3 &v3);
  void SetRotation(const glm::quat &q);

  void SetVisibility(const bool b);
  void SetName(std::string const &name);

  virtual void Update(const double delta);
  virtual void Render();

  void AddComponent(std::unique_ptr<Component> &c);
  void RemoveComponent(Component &c);
  const std::vector<std::unique_ptr<Component>> *GetComponents() const;
  std::vector<Component *> GetComponents(std::string const &name) const;

  template <typename T> T *const getComponent() {
    for (std::vector<Component *>::iterator it = components_.begin(); it != components_.end(); ++it) {
      // printf("Checking %s against %s \n", typeid(**it).name(),
      // typeid(T).name());
      if (&typeid(**it) == &typeid(T)) {
        return static_cast<T *>(*it);
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
