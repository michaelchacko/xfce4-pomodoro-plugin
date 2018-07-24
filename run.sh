#!/bin/bash

automake && ./configure && make && sudo make install && xfce4-panel -r
