# Git Anleitung

## Repository klonen

### Mit SSH
- git clone git@github.com:uwudwagonl/Swiity.git

### Mit HTTPS
- git clone https://github.com/uwudwagonl/Swiity.git


## Remote Repository aktualisieren

- git pull origin main

### Bei Problemen
- git stash
- git clean -fd
- git pull origin main

Wichtig:
- git clean -fd löscht nur ungetrackte Dateien.
- Der Stash wird nicht automatisch gelöscht.
- Mit git stash list kannst du prüfen, ob dein Stash noch existiert.
- Mit git stash pop kannst du ihn wiederherstellen.


## Änderungen hochladen

- git add .
- git commit -m "Nachricht"
- git push origin main

### Bei Problemen
- git pull origin main
- git push origin main

Falls Merge-Konflikte auftreten, müssen diese zuerst manuell gelöst werden.

Tipp:
Vor jedem git push am besten einmal git pull origin main ausführen, um Konflikte zu vermeiden.
