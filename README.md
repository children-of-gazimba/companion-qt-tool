# Companion

Companion is a software toolkit to digitally aid the pen & paper tabletop roleplaying experience. It provides ui based features that should help game masters create and manage media contents, which they wish to incorporate in their game sessions. The codebase and docs for this project are still work-in-progress. Check out our [proceedings from UIST 2018](https://dl.acm.org/citation.cfm?id=3266097) for more info. 

## Companion codebase 

All relevant repos for the current version of Companion can be found at the [Children of Gazimba github page](https://github.com/children-of-gazimba). it might make sense to have a folder on your disk containing all respective code repos you are using in the following structure:
- `<code-folder>\companion-qt-tool`
- `<code-folder>\companion-sqlalchemy-models`
- `<code-folder>\companion-db-utility`
- `<code-folder>\companion-db-migrations`
- and an empty folder `<code-folder>\companion-shared-files`: This one will have to contain the Sqlite database under Mac & OSX. Under Windows this path remains unused.

### Qt Desktop App

To build the Qt C++ app from code it will suffice to clone or fork [Companion Qt tool](https://github.com/children-of-gazimba/companion-qt-tool). The most recent development state is available on the `develop` branch. This is the branch we base all our open issues on. Master is likely a bit outdated, yet should in any case be stable to build and run. 

Make sure to also init and update all submodules in this repo:
```
git clone https://github.com/children-of-gazimba/companion-qt-tool
cd companion-qt-tool
git submodule init
git submodule update --recursive --remote
``` 

Companion supports the [Spotify API](https://developer.spotify.com/documentation/web-api/). In order to auth with the service Companion needs to lookup your API key. This key isn't distributed on github. Companion will expect it at `companion-qt-tool\src\_RES\spotify_credentials.json` with the following content structure.
```
{
  "client_id": <your-api-client-id>,
  "client_secret": <your-api-client-secret>
}
```

Last, you will have to install [Qt Creator](https://www.qt.io/download) to build and run the project. Make sure to install the latest version of Qt when running the Qt installer and also ensure the MinGW compiler kit is installed (to be sure install 32bit as well as 64bit). These options will be available under the component selection of the Qt installer. the companion-qt-tool repo contains a .pro file you can open within Qt creator. Choose your compiler kit (MinGW 32bit was used and verified on Windows & Linux) and configure. Then simply build & run in the Qt Creator IDE.

### Database

For most intents and purposes you will not have to interface with any of the the database git repos. *(Internal) If you use Windows, an up-to-date db will be distributed with the recent installer.* On OSX and Linux, just contact someone who already has a copy of an empty database and put it at the respective database location. For Windows this path is `C:\Users\<username>\AppData\Local\CoG\companion`. Under Mac & Linux it will be `<companion-qt-repo>\..\companion-shared-files`

#### Creating the database from code

We are currently using Sqlalchemy as an object relational database wrapper. It defines the database structurally using Python classes and generates SQL calls under the hood. [Sqlalchemy models](https://github.com/children-of-gazimba/companion-sqlalchemy-models) contains a bunch of db model definitions. This is just a structural description of the database, with a lightweight api to further ease interaction with data from code. The [Database utility](https://github.com/children-of-gazimba/companion-db-utility) includes the models as a git submodule and will help you to create a Sqlite database from code. To do that:
  - clone or fork the [database utility](https://github.com/children-of-gazimba/companion-db-utility), also make sure to setup the submodule of this repo after cloning:
```
git clone https://github.com/children-of-gazimba/companion-db-utility
cd companion-db-utility
git submodule init
git submodule update --recursive --remote
``` 
  - install [virtualenv](https://gist.github.com/Geoyi/d9fab4f609e9f75941946be45000632b) (if necessary)
  - on the same level as the cloned the repo folder, create an empty folder named companion-shared-files
  - open up a terminal inside companion-db-utility and execute the following:
```
virtualenv env
pip install -r requirements.txt
python db_test.py
```
  - a Sqlite database named companion.db should now reside under `companion-db-utility/../companion-shared-files`

We also have setup a work-in-progress [database migrations environment](https://github.com/children-of-gazimba/companion-db-migrations). It hasn't been used until now, because there hasn't been any need to do so. Eventually it can be used to structurally change a running database without losing data. It uses Alembic, which is based upon Sqlalchemy and can autogenerate so called migration scripts (Python). But let's not dig into boring back-end stuff too much. =)

## Companion installer (internal use / Windows only)

The most recent installer.exe will be provided by b00dle for internal use. To install just run the installer and navigate through the wizard. as you will see in the installer, it'll install two components. One is a Sqlite database and the other is the Qt app itself. If you run from code, the (windows) application will search for the local database at the path used by the installer (`C:\Users\<username>\AppData\Local\CoG\companion`). I suggest to leave all suggested paths as they appear in the dialog, to ensure all works as expected. Obviously, for testing and debugging feel free to try and break stuff. 
You should get a desktop and start menu shortcut once the installation is finished. 

To deinstall, navigate to `C:\Program Files (x86)\CoG\companion` (or varying install location) and execute maintanancetool.exe. Proceed to click Remove all Components. All Companion binaries as well as the database will be entirely wiped from your disk.

## Links and resources

This is just an FYI collection of stuff related to what is mentioned above.

- [VoidRealms - Qt C++ programming tutorials](https://www.youtube.com/watch?v=6KtOzh0StTc&list=PL2D1942A4688E9D63): This is a loooooot of content, but covers nearly all aspects of Qt that might come in useful. I found it really helpful to just consult this playlist whenever I look for some new specific feature of Qt. Qt itself offers incredible amounts of well structured docs, examples and a forum, which I would encourage anyone to use in case of very specific Qt related questions. In the past I oftentimes found Qt community answers a lot more helpful, than the ones on stackoverflow. Qt creator also includes an extensive doc of all Qt features, which can be triggered by hitting F1 in the IDE
- [Sqlalchemy tutorials](http://docs.sqlalchemy.org/en/latest/orm/tutorial.html)
- [Alembic tutorials](http://alembic.zzzcomputing.com/en/latest/tutorial.html)
- Pen & Paper tabletop RPG: If you find a way to access it, you might want to check out an episode of Harmon Quest. It is a partially scripted P&P tabletop RGB campaign played by Rick and Morty creator Dan Harmon & friends. Really funny stuff! Plus, it gives you a good idea of the actual gameplay. For german speakers, I also recommend any of the rocketbeans pen and paper campaigns which are freely available on youtube and spotify.