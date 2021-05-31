# TextEditor
Text Editor with Modern Feature and Compression. Port to other x86 Platform other than windows such as linux and macOS should be easy and straightforward because of wxWidgets code compatibility. It should bring minimal to zero changes in code. Sadly, i wasn't able to port to other than windows for now because of time limitation for developing the app.

## GUI (Graphical User-Interface)
The application is made on top of wxWidget library for entire build on x86 platform.
### General Feature
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(3).jpg)
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(4).jpg)
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(5).jpg)

## Fail-Safe and Ease of Use
### Autosave
Autosave feature will automatically save your temporary file so you can close the application without needed to save. This will be convenient for those of you who just want to take a quick notes. During launch, the application will automatically pick all temporary file you've left. This feature is deployed to another thread with a time interval that you can configure. This will ensure you have a checkpoint if your system unexpectedly shutdown or app not responding.
### Temporary File
Create, Edit, and Preview multiple notes add the same time without even bother to save it. Temporary file is automatically loaded during application launch. You don't necessarily need to save your notes because the app is saving it automatically as a temporary file.

## Custom Dictionary
A dictionary that use csv format and certain rules in order to apply as a text highlighting or auto completion. The time to develop this app is very limited we couldn't create in-depth explanation of how to create this. Luckily, we include examples of the dictionary in the installer so you can understand and be able to create one for yourself. 
### Text Highlighting
You can manually configure the color-code of each dictionary from your working pages.
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(1).jpg)
### Auto Completion
All the words in the dictionary will be imported as a word suggestion in your working pages.
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(2).jpg)

## Share over LAN
You can share your text file to other client over Local Area Network. The text editor has built-in share feature so you can connect easily to all client while using the text editor. Any client will communicate through specific port during run-time. This will make your device discoverable to the network without hassle. With this feature, there's notification that let you know if someone wants to share or connect. You can disable this feature in the configuration menu.

## Compression
We use compression to reduce text file. Any file with extension "mtx" considered to be a compressed text files.
### Multi-Threaded Codec
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/multi-thread.jpg)
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/file.jpg)
## Development Report
https://bit.ly/2TexwEA
### Debug and Performance Assesment
This application will log information about performance and error handling. It also equipped with debug console so the developers can test the app while running.
![](https://github.com/ramdanks/TextEditor/blob/main/Preview/feature%20(6).jpg)
