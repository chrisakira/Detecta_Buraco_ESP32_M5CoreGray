/**
 * Copyright (c) 2011-2018 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * \file
 * \brief PrintFile_ class
 */
#ifndef ArduinoFile_s_h
#define ArduinoFile_s_h
#include "FatLibConfig.h"
#if ENABLE_ARDUINO_FEATURES
#include "FatFile.h"
#include <limits.h>
//------------------------------------------------------------------------------
/** Arduino SD.h style flag for open for read. */
#define FILE_READ O_RDONLY
/** Arduino SD.h style flag for open at EOF for read/write with create. */
#define FILE_WRITE (O_RDWR | O_CREAT | O_AT_END)
//==============================================================================
/**
 * \class PrintFile_
 * \brief FatFile_ with Print.
 */
class PrintFile_ : public FatFile_, public Print {
 public:
  PrintFile_() {}
  /**  Create a file object and open it in the current working directory.
   *
   * \param[in] path A path for a file to be opened.
   *
   * \param[in] oflag Values for \a oflag are constructed by a
   * bitwise-inclusive OR of open flags. see
   * FatFile_::open(FatFile_*, const char*, oflag_t).
   */
  PrintFile_(const char* path, oflag_t oflag) : FatFile_(path, oflag) {}
#if DESTRUCTOR_CLOSES_FILE
  ~PrintFile_() {}
#endif  // DESTRUCTOR_CLOSES_FILE
  using FatFile_::clearWriteError;
  using FatFile_::getWriteError;
  using FatFile_::read;
  using FatFile_::write;
  /** \return number of bytes available from the current position to EOF
   *   or INT_MAX if more than INT_MAX bytes are available.
   */
  int available() {
    uint32_t n = FatFile_::available();
    return n > INT_MAX ? INT_MAX : n;
  }
  /** Ensure that any bytes written to the file are saved to the SD card. */
  void flush() {
    FatFile_::sync();
  }
  /** Return the next available byte without consuming it.
   *
   * \return The byte if no error and not at eof else -1;
   */
  int peek() {
    return FatFile_::peek();
  }
  /** Read the next byte from a file.
   *
   * \return For success return the next byte in the file as an int.
   * If an error occurs or end of file is reached return -1.
   */
//  int read() {
//    return FatFile_::read();
//  }
  /** Write a byte to a file. Required by the Arduino Print class.
   * \param[in] b the byte to be written.
   * Use getWriteError to check for errors.
   * \return 1 for success and 0 for failure.
   */
  size_t write(uint8_t b) {
    return FatFile_::write(b);
  }
  /** Write data to an open file.  Form required by Print.
   *
   * \note Data is moved to the cache but may not be written to the
   * storage device until sync() is called.
   *
   * \param[in] buf Pointer to the location of the data to be written.
   *
   * \param[in] size Number of bytes to write.
   *
   * \return For success write() returns the number of bytes written, always
   * \a nbyte.  If an error occurs, write() returns -1.  Possible errors
   * include write() is called before a file has been opened, write is called
   * for a read-only file, device is full, a corrupt file system or an
   * I/O error.
   */
  size_t write(const uint8_t *buf, size_t size) {
    return FatFile_::write(buf, size);
  }
};
//==============================================================================
/**
 * \class File_
 * \brief Arduino SD.h style File_ API
 */
class File_ : public FatFile_, public Stream {
 public:
  File_() {}
  /**  Create a file object and open it in the current working directory.
   *
   * \param[in] path A path with a valid 8.3 DOS name for a file to be opened.
   *
   * \param[in] oflag Values for \a oflag are constructed by a
   * bitwise-inclusive OR of open flags. see
   * FatFile_::open(FatFile_*, const char*, oflag_t).
   */
  File_(const char* path, oflag_t oflag) {
    open(path, oflag);
  }
  using FatFile_::clearWriteError;
  using FatFile_::getWriteError;
  using FatFile_::read;
  using FatFile_::write;
  /** The parenthesis operator.
    *
    * \return true if a file is open.
    */
  operator bool() {
    return isOpen();
  }
  /** \return number of bytes available from the current position to EOF
   *   or INT_MAX if more than INT_MAX bytes are available.
   */
  int available() {
    uint32_t n = FatFile_::available();
    return n > INT_MAX ? INT_MAX : n;
  }
  /** Ensure that any bytes written to the file are saved to the SD card. */
  void flush() {
    FatFile_::sync();
  }
  /** This function reports if the current file is a directory or not.
  * \return true if the file is a directory.
  */
  bool isDirectory() {
    return isDir();
  }
  /** No longer implemented due to Long File_ Names.
   *
   * Use getName(char* name, size_t size).
   * \return a pointer to replacement suggestion.
   */
  const char* name() const {
    return "use getName()";
  }
  /** Return the next available byte without consuming it.
   *
   * \return The byte if no error and not at eof else -1;
   */
  int peek() {
    return FatFile_::peek();
  }
  /** \return the current file position. */
  uint32_t position() {
    return curPosition();
  }
  /** Opens the next file or folder in a directory.
   *
   * \param[in] oflag open oflag flags.
   * \return a File_ object.
   */
  File_ openNextFile_(oflag_t oflag = O_RDONLY) {
    File_ tmpFile_;
    tmpFile_.openNext(this, oflag);
    return tmpFile_;
  }
  /** Read the next byte from a file.
   *
   * \return For success return the next byte in the file as an int.
   * If an error occurs or end of file is reached return -1.
   */
  int read() {
    return FatFile_::read();
  }
  /** Rewind a file if it is a directory */
  void rewindDirectory() {
    if (isDir()) {
      rewind();
    }
  }
  /**
   * Seek to a new position in the file, which must be between
   * 0 and the size of the file (inclusive).
   *
   * \param[in] pos the new file position.
   * \return true for success else false.
   */
  bool seek(uint32_t pos) {
    return seekSet(pos);
  }
  /** \return the file's size. */
  uint32_t size() {
    return fileSize();
  }
  /** Write a byte to a file. Required by the Arduino Print class.
   * \param[in] b the byte to be written.
   * Use getWriteError to check for errors.
   * \return 1 for success and 0 for failure.
   */
  size_t write(uint8_t b) {
    return FatFile_::write(b);
  }
  /** Write data to an open file.  Form required by Print.
   *
   * \note Data is moved to the cache but may not be written to the
   * storage device until sync() is called.
   *
   * \param[in] buf Pointer to the location of the data to be written.
   *
   * \param[in] size Number of bytes to write.
   *
   * \return For success write() returns the number of bytes written, always
   * \a nbyte.  If an error occurs, write() returns -1.  Possible errors
   * include write() is called before a file has been opened, write is called
   * for a read-only file, device is full, a corrupt file system or an
   * I/O error.
   */
  size_t write(const uint8_t *buf, size_t size) {
    return FatFile_::write(buf, size);
  }
};
#endif  // ENABLE_ARDUINO_FEATURES
#endif  // ArduinoFile_s_h
