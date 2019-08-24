# Midnight-Editor-06.2-X-COM-Apocalypse-source-code
Midnight Editor 06.2: X-COM: Apocalypse source code / Copyright (c) 1997-98 Marcin Wichary

How to prepare the code for compiling...

Compiler version 4.x:

1. Unpack the contents of this archive to a directory of your choice
   (I suggest C:\MSDEV\PROJECTS\ME062).
2. Run Microsoft Visual C++ 4.x (this source has been tested only with
   4.0 and 5.0 versions of the compiler).
3. Select File | New... from menu or press Ctrl+N.
4. Select "Project Workspace" and press OK.
5. Select "Application". In "Name" edit enter "ME062". Make sure the
   "Location" directory matches the one you've selected in step 1. Press
   Create.
6. Select Insert | Files into Project... from menu.
7. Select all four files (MAIN.CPP, MAIN.H, MAIN.RC and RESOURCE.H) by
   pressing Ctrl on each one of them, then press Add.
8. Select Build | Settings... from menu or press Alt+F7.
9. Select "Link" tab and ADD "comctl32.lib" (without quotation marks) to
   "Object/library modules" edit control. Press OK.
10. Now you are ready to run program by pressing Ctrl-F5. Enjoy!

Compiler version 5.0:

1. Unpack the contents of this archive to a directory of your choice
   (I suggest C:\Program Files\MyProjects\DevStudio\ME062).
2. Run Microsoft Visual C++ 5.0 (this source has been tested only with
   4.0 and 5.0 versions of the compiler).
3. Select File | New... from menu or press Ctrl+N.
4. Select "Projects" tab (it should be selected by default). Choose 
   "Win32 Application". In "Project name" edit enter "ME062". Make 
   sure the "Location" directory matches the one you've selected in 
   step 1. Press OK.
6. Select Project | Add To Project | Files... from menu.
7. Select all four files (MAIN.CPP, MAIN.H, MAIN.RC and RESOURCE.H) by
   pressing Ctrl on each one of them, then press OK.
8. Select Project | Settings... from menu or press Alt+F7.
9. Select "Link" tab and ADD "comctl32.lib" (without quotation marks) to
   "Object/library modules" edit control. Press OK.
10. Now you are ready to run program by pressing Ctrl-F5. Enjoy!

In case of any problems, etc. do not hesitate to contact me at
mwichary@polbox.com and/or mwichary@friko2.onet.pl. Good luck!
