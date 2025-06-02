# Eidolon

**Eidolon** is a lightweight desktop app that connects to `llama-cli` to dynamically generate answers using a local LLM model.

## Features
- **Dynamic Streaming**: Answers appear word by word in real-time.
- **Automatic Model Selection**: Uses the first `.gguf` model found in the `Models/` folder.
- **CUDA Acceleration**: Offloads 29 layers to GPU if available.
- **Portable**: No external dependencies needed—just unzip and run!

## Usage
1. Unzip `Eidolon.zip`.
2. Double-click `Eidolon.exe`.
3. Type your prompt and press Enter.

## Explanation
This project:
- Uses **Qt** for the UI.
- Uses **llama.cpp**’s `llama-cli.exe` to run local LLM inference.
- Dynamically detects and uses the first `.gguf` model in the `Models/` folder.
- Uses **relative paths** to keep everything portable.

The actual LLM inference is done by `llama-cli.exe`, which is built separately from the official `llama.cpp` repo. `Eidolon` simply provides a **friendly UI** for easy interaction!

---
