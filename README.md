# XOR-Dhyan-Joshi
XOR- encryption decryption application using C++
# 🔐 XOR File Encryptor CLI

A lightweight C++ command-line application to **encrypt** and **decrypt** files using XOR cipher and a binary key.

---

## 📦 Project Name
**XOR**

---

## 🚀 Features

- Fast XOR-based encryption and decryption
- Works with any file type (text, binary, etc.)
- Uses user-supplied key file (for strong XOR cipher)
- Minimal external dependencies
- CLI-driven with batch script support

---

## 🛠️ Prerequisites

- Windows with g++ installed (via MinGW or WSL)
- C++17 compatible compiler
- Required headers:
  - `<fstream>`
  - `<vector>`
  - `<string>`
  - `<iostream>`
  - `<filesystem>`

---

## ⚙️ Building the App

```bash
g++ xor_encryptor.cpp -o xor_encryptor -std=c++17
