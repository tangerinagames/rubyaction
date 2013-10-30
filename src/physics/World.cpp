#include "physics/World.hpp"
#include "RubyEngine.hpp"
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/variable.h>

namespace RubyAction
{
namespace Physics
{

  World::World(mrb_value self, int gravityx, int gravity, bool doSleep)
    : EventDispatcher(self)
  {
    this->world = new b2World(b2Vec2(gravityx, gravity));
    this->world->SetAllowSleeping(doSleep);
  }

  World::~World()
  {
    delete this->world;
  }

  void World::clearForces()
  {
    this->world->ClearForces();
  }

  int* World::getGravity()
  {
    b2Vec2 gravity = this->world->GetGravity();
    return new int[2] { static_cast<int>(gravity.x), static_cast<int>(gravity.y) };
  }

  void World::setGravity(int gravityx, int gravity)
  {
    this->world->SetGravity(b2Vec2(gravityx, gravity));
  }

  void World::step(float timeStep, int velocityIterations, int positionIterations)
  {
    this->world->Step(timeStep, velocityIterations, positionIterations);
  }

  void World::BeginContact(b2Contact* contact)
  {
    mrb_state *mrb = RubyEngine::getInstance()->getState();
    mrb_value data[] = { mrb_fixnum_value(1), mrb_fixnum_value(2) };
    this->dispatch(mrb_intern(mrb, "begin_contact"), data, 2);
  }

  void World::EndContact(b2Contact* contact)
  {
    mrb_state *mrb = RubyEngine::getInstance()->getState();
    mrb_value data[] = { mrb_fixnum_value(1), mrb_fixnum_value(2) };
    this->dispatch(mrb_intern(mrb, "end_contact"), data, 2);
  }

  static mrb_value World_initialize(mrb_state *mrb, mrb_value self)
  {
    mrb_int gravityx;
    mrb_int gravityY;
    mrb_bool doSleep;
    int argc = mrb_get_args(mrb, "ii|b", &gravityx, &gravityY, &doSleep);

    if (argc == 2) doSleep = true;

    SET_INSTANCE(self, new World(self, gravityx, gravityY, doSleep));
    return self;
  }

  static mrb_value World_clearForces(mrb_state *mrb, mrb_value self)
  {
    GET_INSTANCE(self, world, World)
    world->clearForces();
    return self;
  }

  static mrb_value World_getGravity(mrb_state *mrb, mrb_value self)
  {
    GET_INSTANCE(self, world, World)
    int* xy = world->getGravity();
    mrb_value gravity[2] = { mrb_fixnum_value(xy[0]), mrb_fixnum_value(xy[1]) };
    return mrb_ary_new_from_values(mrb, 2, gravity);
  }

  static mrb_value World_setGravity(mrb_state *mrb, mrb_value self)
  {
    mrb_value gravity;
    mrb_get_args(mrb, "A", &gravity);

    GET_INSTANCE(self, world, World)
    world->setGravity(mrb_fixnum(mrb_ary_ref(mrb, gravity, 0)), mrb_fixnum(mrb_ary_ref(mrb, gravity, 1)));
    return self;
  }

  static mrb_value World_step(mrb_state *mrb, mrb_value self)
  {
    mrb_float timeStep;
    mrb_int velocityIterations;
    mrb_int positionIterations;
    mrb_get_args(mrb, "fii", &timeStep, &velocityIterations, &positionIterations);

    GET_INSTANCE(self, world, World)
    world->step(timeStep, velocityIterations, positionIterations);
    return self;
  }

  void bindWorld(mrb_state *mrb, RClass *module, RClass *physics)
  {
    struct RClass *super = mrb_class_get_under(mrb, module, "EventDispatcher");
    struct RClass *clazz = mrb_define_class_under(mrb, physics, "World", super);
    MRB_SET_INSTANCE_TT(clazz, MRB_TT_DATA);

    mrb_define_method(mrb, clazz, "initialize", World_initialize, MRB_ARGS_ARG(2, 1));
    mrb_define_method(mrb, clazz, "clear_forces", World_clearForces, MRB_ARGS_NONE());
    mrb_define_method(mrb, clazz, "gravity", World_getGravity, MRB_ARGS_NONE());
    mrb_define_method(mrb, clazz, "gravity=", World_setGravity, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, clazz, "step", World_step, MRB_ARGS_REQ(3));
  }

}
}
