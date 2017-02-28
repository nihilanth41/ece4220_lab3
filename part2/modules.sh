#!/bin/bash

rmmod rtai_fifos rtai_lxrt 
insmod /usr/realtime/modules/rtai_sem.o # rtai_ksched rtai_sem
