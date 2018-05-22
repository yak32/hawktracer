#include "chrome_trace_converter.hpp"

static uint64_t ns_to_ms(uint64_t nano_secs)
{
    return nano_secs / 1000u;
}

namespace HawkTracer
{
namespace client
{

ChromeTraceConverter::~ChromeTraceConverter()
{
    uninit();
}

bool ChromeTraceConverter::init(const std::string& file_name)
{
    _file.open(file_name);
    if (_file.is_open())
    {
        _file << "{\"traceEvents\": [ {} ";
        return true;
    }
    return false;
}

void ChromeTraceConverter::uninit()
{
    if (_file.is_open())
    {
        _file << "]}";
        _file.close();
    }
}

void ChromeTraceConverter::process_event(const parser::Event& event)
{
    std::string label = get_label(event);

    // Chrome expects the timestamps/durations to be microseconds
    // so we need to convert from nano to micro
    _file << ",{\"name\": \"" << label
         << "\", \"ph\": \"X\", \"ts\": " << ns_to_ms(event.get_value<uint64_t>("timestamp"))
         << ", \"dur\": " << ns_to_ms(event.get_value_or_default<uint64_t>("duration", 0u))
         << ", \"pid\": 0, \"tid\": " << event.get_value_or_default<uint32_t>("thread_id", 0)
         << "}";
}

bool ChromeTraceConverter::stop()
{
    uninit();
    return true;
}

} // namespace client
} // namespace HawkTracer
