# TextEditor
Text Editor with Cross Platform support and Compression feature.

## GUI (Graphical User-Interface)
The application is made on top of wxWidget library for entire build on x86 platform.

## Autosave
Autosave feature will automatically save your temporary file so you can close the application without needed to save. This will be convenient for those of you who just want to take a quick notes. During launch, the application will automatically pick all temporary file you've left. This feature is deployed to another thread with a time interval that you can configure. This will ensure you have a checkpoint if your system unexpectedly shutdown or app not responding.

## Temporary File
A temporary file can help you if you want to have a multiple quick notes. You can decide later if you want to save your notes because it is save to close without saving first. If you feel you have done with it, you can close the page and it will automatically delete that temporary file.

## Text Highlighting
This feature will color code some text in your document. You need to make a dictionary first and then embed it to your file. We will also provide built-in dictionary that you can use. The dictionary as well as the text file can be edited  in this application at the same time. You can manually configure the color-code of each dictionary from your working pages. Dictionary will also contain a description, and default color for all the text related to the dictionary. Dictionary can be merged to your text files so everyone can enjoy a color-coded document without manually need to embed them.

## Share over LAN
You can share your text file to other client over Local Area Network. The text editor has built-in share feature so you can connect easily to all client while using the text editor. Any client will communicate through specific port during run-time. This will make your device discoverable to the network without hassle. With this feature, there's notification that let you know if someone wants to share or connect. You can disable this feature in the configuration menu.

## Compression
We use compression to reduce text file. Any file with extension "mtx" considered to be a compressed text files.
### Huffman-Code Algorithm
### Clusterization

## Development Report
https://bit.ly/2TexwEA
