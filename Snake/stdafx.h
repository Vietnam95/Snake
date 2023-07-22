#pragma once
#pragma warning(disable : 4996)
#include <iostream>
#include <vector>
#include <conio.h>
#include <cstring>
#include <time.h>
#include <random>
#include <memory>
#include <map>
#include <string>
#include <deque>
#include <fstream>

// boost
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <thread>
#include <atomic>
#include <iterator>
#include <windows.h>
#include <stdlib.h>
#include "jsoncons/json.hpp"