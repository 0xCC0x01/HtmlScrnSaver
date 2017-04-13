# HtmlScrnSaver
ScreenSaver that display web page/local HTML

I use this to display my [google_io_clock](https://github.com/0xCC0x01/google_io_clock), it's a cool digital clock screensaver :)

# Handling Screen Savers
- To create a screen saver, must create source code containing three required functions and link them with the screen saver library.

    - `ScreenSaverProc`
        - This function processes specific messages and passes any unprocessed messages back to the screen saver library.
        - You must link your project with screen saver library: Scrnsave.lib (or Scrnsavw.lib for unicode)
    - `ScreenSaverConfigureDialog`
        - This function displays a dialog box that enables the user to configure the screen saver (an application must provide a corresponding dialog box template). Windows displays the configuration dialog box when the user selects the Setup button in the Control Panel's Screen Saver dialog box.
        - You must define the constant used to identify the dialog box template by using the decimal value 2003
    - `RegisterDialogClasses`
        - This function must be called by all screen saver applications. However, applications that do not require special windows or custom controls in the configuration dialog box can simply return TRUE. Applications requiring special windows or custom controls should use this function to register the corresponding window classes.

- To install/use a screen saver, must rename it to .scr extensions (or you could change project settings to generate .scr instead of .exe) and copy to the correct directory, Control Panel will search the directory for files with .scr extension.