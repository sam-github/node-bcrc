#include <boost/crc.hpp>
#include <inttypes.h>

/*
CRC wrapper, implementing a CRC interface. This is a work-around for the
templatization of boost/crc, which creates a different type per CRC width.
*/
class Crc
{
    public:
        virtual ~Crc() {};
        virtual void reset() = 0;
        virtual void process_bytes(const void* buffer, size_t byte_count) = 0;
        virtual uintmax_t checksum() const = 0;
};

template < std::size_t Bits >
class CrcBasic : public Crc
{
    private:

        boost::crc_basic<Bits> crc_;

    public:

        CrcBasic(
                 int truncated_polynominal,
                 int initial_remainder,
                 int final_xor_value,
                 bool reflect_input,
                 bool reflect_remainder
            ) : crc_(
                 truncated_polynominal,
                 initial_remainder,
                 final_xor_value,
                 reflect_input,
                 reflect_remainder
            )
        {
        }

        ~CrcBasic() {};

        void reset()
        {
            crc_.reset();
        }

        void process_bytes(const void* buffer, size_t byte_count)
        {
            crc_.process_bytes(buffer, byte_count);
        }

        uintmax_t checksum() const
        {
            return crc_.checksum();
        }
};

template < class Optimal >
class CrcOptimal : public Crc
{
    private:

        Optimal crc_;

    public:

        ~CrcOptimal() {};

        void reset()
        {
            crc_.reset();
        }

        void process_bytes(const void* buffer, size_t byte_count)
        {
            crc_.process_bytes(buffer, byte_count);
        }

        uintmax_t checksum() const
        {
            return crc_.checksum();
        }
};

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
  return args[arg]->BooleanValue();
}

Handle<Value> BcrcObject::Basic(const Arguments& args) {
  HandleScope scope;

  if (args.Length() < 2) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }

    int bits = CheckInt32(args, 1);
    int poly = CheckInt32(args, 2);
    int initial = OptInt32(args, 3, 0);
    int xor_ = OptInt32(args, 4, 0);
    int reflect_input = CheckBool(args, 5);
    int reflect_remainder = CheckBool(args, 6);

    Crc* crc = NULL;

    switch(bits) {
        case  8: crc = new CrcBasic< 8>(poly, initial, xor_, reflect_input, reflect_remainder); break;
        case 16: crc = new CrcBasic<16>(poly, initial, xor_, reflect_input, reflect_remainder); break;
        case 24: crc = new CrcBasic<24>(poly, initial, xor_, reflect_input, reflect_remainder); break;
        case 32: crc = new CrcBasic<32>(poly, initial, xor_, reflect_input, reflect_remainder); break;
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
