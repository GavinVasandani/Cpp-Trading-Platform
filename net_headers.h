#pragma once
/*This is a file with all the headers, we can just call this file in the beginning of a new file to use the headers
instead of rewriting all headers*/
#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include<vector>
#include<iostream>
#include<algorithm>
#include<chrono>
#include<cstdint>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>