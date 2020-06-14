# alt-grab

Alt-Grab is a small utility program to very quickly switch between applications or windows (on a windows machine). 

I found that I was leaning heavily on alt-tab for day to day work, but in reality was only switching between a few open programs. Using alt-tab for that was consistently frustrating, given that my intuition about the alt-tab order only seemed to be correct about half the time if I needed to switch frequently between more than 2 different applications. 

Alt-Grab solves this by letting you bind any open window to one of four "slots." Once a window has been bound to a slot, you can bring it immediately to the foreground with a key command. You can use the same key command to dismiss that slot's window if it's already open (and return to the previously open window). 


### How to Use

Alt-grab lives in the system tray. Once it's started, you interact with it solely through key commands (unless you want to exit, which you do through the tray icon). 

| Key Command | Action | 
|-------------|--------|
| Left Alt + Shift + j | Bind current window to slot 'J' |
| Left Alt + Shift + k | Bind current window to slot 'K' |
| Left Alt + Shift + l | Bind current window to slot 'L' |
| Left Alt + Shift + ; | Bind current window to slot ';' |
| Left Alt + j | Summon/Dismiss window bound to slot 'J'|
| Left Alt + k | Summon/Dismiss window bound to slot 'K'|
| Left Alt + l | Summon/Dismiss window bound to slot 'L'|
| Left Alt + ; | Summon/Dismiss window bound to slot ';'|

Only one window can be bound to a slot at a time. If you bind a window to a slot that is already bound, that slot's contents will be replaced.

### Notes

I don't expect this will be useful to anyone other than myself, but if you DO use it, know that there are a few limitations on the current version: 

* You can't use this to bring a window up if the foregroud window is the task manager, regedit, or a number of other special windows programs

* The logic for dismissing a foregrounded window slot by pressing it's key combination again isn't 100% bulletproof, and won't work right if that slot's window was brought to the foreground by any method other than it's alt-grab shortcut. 


