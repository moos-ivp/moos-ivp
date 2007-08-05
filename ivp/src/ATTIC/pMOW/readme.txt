Dear All

Right  - pMOW is in place (checked in under MOOS/Devel). This is a copy of the readme in the project directory....




Task1: Simulate a drifting navigation algorithm.
***********************************************

It does this by subscribing to [NavPrefix]_X,Y,YAW etc and adding brownian noise (parameterised by XYDriftRate ). The "drifted" navigation is published with a [DriftPrefix] prefix. So one might have in real life "NavPrefix  = GPS" and "DriftPrefix = DRIFT". In this case this value would be corrupted and republished under "DRIFT_X". Comprende?


Task2: Produce a set of probable locations for a vehicle given a set of two-way TOF pings.
***********************************************

It does this by subscribing to the "PingName" variable for example NAV_KM_LBL_TOF which currently must be of the form

Tx = time,Ch[x] = tof,Ch[y] = tof..... (where x and y are channel numbers/addresses)

These tofs are combined with nav info (see above) to produce a set of possible locations for the vehicles sporting the modem replying on address x and y etc. So if there were only one other vehicle in the water the NAV_KM_LBL_TOF will only have one Ch[x]=tof field and when possible pMOW will start publishing upto thirty locations for the vehicle carrying Ch[x].

The locations are are ranked and the results string has the following format:

     Channel = nChan, Target = [2x1]{x0,y0},Scores = [3xn]{x0,y0,s0,x1,y1,s1,....}

where:
   Channel is the ID of the target modem (the one responding to pings)
   x0 and y0 are the coordinates of the most probable location of the target vehicle
   Scores is flattened,sorted 3xn matrix of coordinates and score in descending order where n<=30 

Notes:

1) One should use two calls to the GenLib MOOSValFromString method to extract the Target and Scores vectors). Alternatively you could just use one call and extract the best "Target".

2)  if the vehicle hosting pMOW moves in a straight line there will be a left right ambiguity in the location of teh target vehicle - this is precisely what one see in the output. Horrah! ALos pretty obviously you have to be moving for a solution to emerge...

3) Note that pMOW does this calculation with no need for an initial guess - i.e it doesn't use least squares iterations.

4) The idea of the Scores vector is it forms the basis of an objective function that IMPLICITY causes resolution of baseline ambiguity - I quite like that....




EXAMPLE CONFIG BLOCKS
****************************


ProcessConfig = pMOW
{
     NavPrefix = NAV_KM
     DriftPrefix = DRIFT
     XYDriftRate = 0.15
     ResetFlag = RESTART_HELM
     PingName = NAV_KM_LBL_TOF
}

to go with this uMVS block

ProcessConfig = uMVS
{

    ADD_AUV=  pose=[4x1]{000,150,55,0},name = Km,InputPrefix=,OutputPrefix=NAV_KM_,ResponderChannel = Ch2,TAT = 0.25
    ADD_AUV=  pose=[4x1]{150,0,55,0},name = Kp,InputPrefix=LALA,OutputPrefix=NAV_KP_,ResponderChannel = Ch3,TAT = 0.125
    TideHeight = 60

    //a few variables for the simulator..
    LogFile = SimLog.txt
    InstantLogAcoustics = false

    //what is standard deviation of noise on
    //TOF measurements? 1ms = 1.5 meters
    TOFNoise = 0.00066
}


Note the AUV Kp won't move in this example under iRemote control - it has been told to subscribe to "LALA_DESIRED_THRUST" etc. So in this case running up uMVS, iRemote and pMOW will allow the user to steer Km around and pMOW should indicate the position of Kp.



------------------------------------