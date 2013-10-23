#include "bcrc.h"

#include <node.h>

using namespace v8;

class BcrcObject : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);

 private:
  Crc* crc_;

  BcrcObject(Crc* crc) : crc_(crc) {};
  ~BcrcObject() { delete crc_; };

  void method() {
    printf("method\n");
  };

  static v8::Handle<v8::Value> Basic(const v8::Arguments& args);
  static v8::Handle<v8::Value> Method(const v8::Arguments& args);
};

void BcrcObject::Init(Handle<Object> exports) {
  // Constructor
  Local<FunctionTemplate> basicTemplate = FunctionTemplate::New(Basic);
  basicTemplate->SetClassName(String::NewSymbol("Bcrc"));
  basicTemplate->InstanceTemplate()->SetInternalFieldCount(1);
  // XXX field count unexplained... what is it? number of sets in the prototype?

  // Prototype
  basicTemplate->PrototypeTemplate()->Set(
    String::NewSymbol("method"),
    FunctionTemplate::New(Method)->GetFunction()
  );

  // XXX why persistent? it didn't need to be persistent before
  Persistent<Function> basicConstructor =
    Persistent<Function>::New(basicTemplate->GetFunction());

  exports->Set(String::NewSymbol("Basic"), basicConstructor);
}

int32_t CheckInt32(const Arguments& args, int arg)
{
  if (!args[arg]->IsNumber()) {
    //ThrowException(Exception::TypeError(String::New("Arg N is expected to be a number")));
    //return scope.Close(Undefined());
    // XXX Wtf? How to do this.
    return 0;
  }
  return args[arg]->Int32Value();
}

int32_t OptInt32(const Arguments& args, int arg, int32_t def)
{
  if (!args[arg]->IsUndefined()) {
    return def;
  }

  return CheckInt32(args, arg);
}

int CheckBool(const Arguments& args, int arg)
{
// XXX does this do what I think?
  return args[arg]->BooleanValue();
}

Handle<Value> BcrcObject::Basic(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

  int bits = CheckInt32(args, 0);
  int poly = CheckInt32(args, 1);
  int initial = OptInt32(args, 2, 0);
  int xor_ = OptInt32(args, 3, 0);
  int reflect_input = CheckBool(args, 4);
  int reflect_remainder = CheckBool(args, 5);

  Crc* crc = NULL;

  switch(bits) {
    case  8:
      crc = new CrcBasic< 8>(poly, initial, xor_, reflect_input, reflect_remainder);
      break;
    case 16:
      crc = new CrcBasic<16>(poly, initial, xor_, reflect_input, reflect_remainder);
      break;
    case 24:
      crc = new CrcBasic<24>(poly, initial, xor_, reflect_input, reflect_remainder);
      break;
    case 32:
      crc = new CrcBasic<32>(poly, initial, xor_, reflect_input, reflect_remainder);
      break;
    default:
      // XXX throw error. default: return luaL_argerror(L, 2, "unsupported crc bit width");
      ThrowException(Exception::TypeError(String::New("Unsupported crc bit width")));
      return scope.Close(Undefined());
  }

  BcrcObject* obj = new BcrcObject(crc);
  obj->Wrap(args.This());

  return args.This();
}

Handle<Value> BcrcObject::Method(const Arguments& args) {
  HandleScope scope;

  BcrcObject* obj = ObjectWrap::Unwrap<BcrcObject>(args.This());

  obj->method();

  return args.This();
}

void InitAll(Handle<Object> exports) {
  BcrcObject::Init(exports);
}

NODE_MODULE(bcrc, InitAll)
