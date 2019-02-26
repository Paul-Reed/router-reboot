# router-reboot
This sketch performs the following actions;

1) Connects to local wifi
2) Every 20 minutes, it pings 'google' (and if no reply - then 'cloudflare') and if it gets
   a response, the ESP will go to deep sleep mode for 20 mins, then repeat.
3) If no reply is received from either, the sketch enters a loop, rechecking google &
   cloudflare, for a further 4 times, 1 minute apart.
4) If no reply is received, the relay will remove power to the router for 5
   seconds causing a router reboot.
5) ESP will then go into deep sleep for 5 minutes to allow the router to
   stabilise.
