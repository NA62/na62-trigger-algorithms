# Guideline

If you want to develop trigger algorithms that should run in the PC-farm, you need to clone this repository and na62-farm-lib. As these are eclipse projects you should import them into your eclipse workspace. Testing the algorithms should be done by cloning [[na62-trigger-test|https://github.com/NA62/na62-trigger-test/wiki] which links this project and na62-farm-lib.

# Getting the code
For a more advanced way to clone the code within the PC-farm see [[this|https://github.com/NA62/na62-farm/wiki/Development]]
## Forking within github
The best way to start with your trigger implementations is to create a github account and then fork this project (See [[this|https://github.com/NA62/na62-trigger-algorithms/fork]] link on the top right hand corner).

Then you can clone your forked repository via (please adjust YourUserName with your github user name)
```
git clone git@github.com:YourUserName/na62-trigger-algorithms.git
```

Don't forget to upload your ssh-key before cloning!

After you've implemented your changes you can send a Pull Request. I will have a look at your commits and accept the request asap.

## Cloning directly
If you don't want to contribute to the project but only run it you can directly clone the code via 
```
git clone https://github.com/NA62/na62-trigger-algorithms.git
```