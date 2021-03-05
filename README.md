# Simulated Experiments for the paper: Runtime Verification of Behavior Trees.

In this repository, you will find the code related to the experiments of the paper: <br>
"Formalizing the Execution Context of Behavior Trees
for Runtime Verification of Deliberative Policies" <br>
Submitted at IROS2021 by Michele Colledanchise, Giuseppe Cicala, Daniele Domenichelli, Armando Tacchella, and Lorenzo Natale.




## Dependencies

- [Docker](https://www.docker.com/)
- [YARP](https://www.yarp.it/) (Optional)



## Setup (Tested with Ubuntu 18.04 and Debian Testing)



1. **Install Docker**
   
Follow the instructions at https://docs.docker.com/install/
   
2. **install Docker Compose**

   Open a terminal window and run:

   
   ```bash
   sudo curl -L "https://github.com/docker/compose/releases/download/1.26.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
   sudo chmod +x /usr/local/bin/docker-compose
   sudo usermod -aG docker $USER
   ```

   If you want docker-compose bash completion run:

   ```bash
   sudo curl -L https://raw.githubusercontent.com/docker/compose/1.26.2/contrib/completion/bash/docker-compose -o /etc/bash_completion.d/docker-compose
   ```

3. **Reboot your system**

4. **Download the software to run the experiment**

   Open a terminal window and run:

   ```bash
   git clone https://github.com/SCOPE-ROBMOSYS/ICRA2021-experiments
   cd ICRA2021-experiments
   sed -i "s/1000/$(id -u)/g" docker-compose.yml # This ensures that the image is built with your current user id
   docker-compose pull scope
   docker-compose build bt-implementation
   ```

   If YARP  is not installed in the system then run, specifying your ip address:

   ```bash
   mkdir -p ${HOME}/.config/yarp/ ${HOME}/.local/share/yarp/
   docker-compose run -T --rm bt-implementation yarp conf <ip address> 10000
   ```

### Execution

Open a terminal  window and the run

```bash
docker-compose up
```

Several windows will open as in the figure below. 

![scope-up](https://user-images.githubusercontent.com/8132627/77664063-18bfb200-6f7e-11ea-8da2-5ed9f36f358c.png)

The experiment starts as soon as all the components are up an running.



To change  the battery level, open a terminal window in the folder `ICRA2021-experiments`  and run

```bash
docker-compose run --rm bt-implementation yarp rpc /fakeBattery/control/rpc:i
```

then write

```
setBatteryCharge X
```

with `X` the new value of the battery level (between 0 an 100), as below:

![image](https://user-images.githubusercontent.com/8132627/97735403-95011180-1ada-11eb-87e7-1215125df40a.png)


## Run Experiment 1

To run Experiment 1 of the paper, change the battery level to 10%



## Run Experiment 2

To run Experiment 2 of the paper, move to the branch `experiment-2` (`git checkout experiment-2`), rebuild (`docker-compose build bt-implementation`), rerun (`docker-compose up`) and change the battery level to 25%






