<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents** 
- [Guideline](#guideline)
- [Getting the code](#getting-the-code)
  - [Forking within github](#forking-within-github)
  - [Cloning directly](#cloning-directly)
- [dependencies](#dependencies)
  - [Compiler](#compiler)
- [Compiling](#compiling)
  - [Installing Eclipse](#installing-eclipse)
  - [Importing the projects](#importing-the-projects)
  - [Building and running](#building-and-running)
  - [Configuring trigger-test](#configuring-trigger-test)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

# Guideline

If you want to develop trigger algorithms that should run in the PC-farm, you need to clone this repository and na62-farm-lib. As these are eclipse projects you should import them into your eclipse workspace. Testing the algorithms should be done by cloning [[na62-trigger-test|https://github.com/NA62/na62-trigger-test/wiki]] which links this project and na62-farm-lib.

# Getting the code
For a more advanced way to clone the code within the PC-farm see [[this link|https://github.com/NA62/na62-farm/wiki/Development]]
## Forking within github
The best way to start with your trigger implementations is to create a github account and then fork this project (See [[this link|https://github.com/NA62/na62-trigger-algorithms/fork]] on the top right hand corner).

Then you can clone your forked repository via (please adjust YourUserName with your github user name)
```
git clone git@github.com:YourUserName/na62-trigger-algorithms.git
```

Don't forget to upload your ssh-key before cloning!

After you've implemented your changes you can send a Pull Request. I will have a look at your commits and accept the request asap.

## Cloning directly
If you don't want to contribute to the project but only run it you can directly clone the code via 
```
git clone https://github.com/NA62/na62-farm-lib.git
```

# dependencies
The trigger-algorithms project is only a library linked by na62-trigger-test and linking another library called na62-farm-lib. To install these dependencies please do following:
```
git clone https://github.com/NA62/na62-trigger-algorithms.git
git clone https://github.com/NA62/na62-trigger-test.git
```

## Compiler
So far all na62-farm projects are tested with gcc 4.8 and 4.9. As c++11 is heavily used it is not to use any gcc compiler below 4.7.

On SLC6 with AFS running you can activate gcc 4.9 as follows
```
source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.0/x86_64-slc6/setup.sh
```

It is recommended to add this to your .bashrc:
```
echo "source /afs/cern.ch/sw/lcg/contrib/gcc/4.9.0/x86_64-slc6/setup.sh" >> ~/.bashrc
```

# Compiling
## Installing Eclipse
As all projects are Eclipse projects, please install Eclipse with CDT ([[link|https://www.eclipse.org/downloads/]]):
```
wget http://ftp.fau.de/eclipse/technology/epp/downloads/release/luna/R/eclipse-cpp-luna-R-linux-gtk-x86_64.tar.gz
sudo tar -xvzf eclipse-cpp*.tar.gz -C /usr/share/
ln -s /usr/share/eclipse /usr/bin/
```

## Importing the projects
In eclipse click `File->Import->General->"Existing Projects into Workspace"`

![](https://raw.githubusercontent.com/NA62/na62-farm-docs/master/wiki-images/eclipse/importProjects.png)

Now click `Browse` and select the folder with all the cloned projects. After pressing `OK` you should see something like this:

![](https://raw.githubusercontent.com/NA62/na62-farm-docs/master/wiki-images/eclipse/importProjects2.png)

Now Click Finish and you should see the projects in the "Project Explorer" view.

## Building and running
For the initial building you should select any cpp file of the na62-trigger-test project and click `Project->Build Project` or build all Projects by pressing `CTRL+b`.

If you want to test trigger algorithms you have to execute the [[trigger-test project|https://github.com/NA62/na62-trigger-test/wiki]] project. Therefore select any file out of this project and click the green run button at the bar at the top.

## Configuring trigger-test
 Some of the available options for this program are:
<dl>
  <dt>rawInputPath</dt>
  <dd>Path to the directory containing all the raw binary files to be read</dd>
  <dt>activeSourceIDs</dt>
  <dd>List of Source IDs to be used</dd>
  <dt>maxEventNum</dt>
  <dd>Maximum number of events to be processed</dd>
</dl>
To specify these options while running within eclipse, you should click the right button next to the run button and then `Run Configurations`:

![](https://raw.githubusercontent.com/NA62/na62-farm-docs/master/wiki-images/eclipse/runConfigs.png)

After selecting the `na62-trigger-test` under `C/C++ Application` you need to select the `Arguments` tab where you can add all the arguments you like:

![](https://raw.githubusercontent.com/NA62/na62-farm-docs/master/wiki-images/eclipse/runConfigs2.png)