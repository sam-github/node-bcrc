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

