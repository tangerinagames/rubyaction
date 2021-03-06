#include "FontBase.hpp"

using namespace RubyAction;

FontBase::FontBase(mrb_value self)
  : RubyObject(self)
{
}

static mrb_value FontBase_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_raise(mrb, E_RUNTIME_ERROR, "The FontBase can't be instantiated. Please use one of subclasses.");
  return self;
}

void RubyAction::bindFontBase(mrb_state *mrb, RClass *module)
{
  struct RClass *clazz = mrb_define_class_under(mrb, module, "FontBase", mrb->object_class);
  MRB_SET_INSTANCE_TT(clazz, MRB_TT_DATA);

  mrb_define_method(mrb, clazz, "initialize", FontBase_initialize, MRB_ARGS_NONE());
}
