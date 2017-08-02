# Installation

`sudo make install` does not make the plugin appear in the menu for some reason.

It seems the installation expects plugins to be installed in `/usr/lib/xfce4/panel-plugins`, but `sudo make install` just puts them in `/usr/local/lib/xfce4/...`
To fix this issue, we symlink the /usr/local/ files to their expected installation locations 

```
sudo ln -s /usr/local/lib/xfce4/panel/plugins/libxfcepomodoro.so /usr/lib/xfce4/panel-plugins/libxfcepomodoro.so
sudo ln -s /usr/local/share/xfce4/panel/plugins/xfce4-pomodoro-plugin.desktop /usr/share/xfce4/panel/plugins/xfce4-pomodoro-plugin.desktop
```

