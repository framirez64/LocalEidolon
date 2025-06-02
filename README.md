# Eidolon

**Eidolon** is a lightweight desktop app that connects uses llama.cpp for hosting LLMs locally on a machine.

## Features
- **Dynamic Streaming**: Answers appear word by word in real-time.
- **Automatic Model Selection**: Uses the first `.gguf` model found in the `Models/` folder.
- **CUDA Acceleration**: Offloads 29 layers to GPU if available.
- **Portable**: No external dependencies needed—just unzip and run!

# Preparation
- This project uses quantized models, so you're able to utilize any model that's formatted with a .gguf ending.
- I recommend using QWEN3-1.7b, or something similarly lightweight.
- The download for that can be found here: https://huggingface.co/Qwen/Qwen3-1.7B-GGUF/tree/main
- After its downloaded, just drag it into the Model folder within the release
## Usage

1. Unzip `Eidolon.zip`.
2. Find the model you downloaded earlier
3. Create a folder called "Models" if it doesn't exist already
4. Drag and drop your chosen .gguf quantized model
5. Double-click "Eidolon.exe"
6. The application will appear within your system trap, but can be right clicked to be made bigger
7. Type your prompt and press Enter.

## Explanation
This project:
- Uses **Qt** for the UI.
- Uses **llama.cpp**’s `llama-cli.exe` to run local LLM inference.
- Dynamically detects and uses the first `.gguf` model in the `Models/` folder.
- Uses **relative paths** to keep everything portable.

The actual LLM inference is done by `llama-cli.exe`, which is built separately from the official `llama.cpp` repo. `Eidolon` simply provides a **friendly UI** for easy interaction!

---
