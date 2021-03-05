/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

enum GoToStatus
{
    NOT_STARTED,
    RUNNING,
    SUCCESS,
    ABORT
}

service GoTo {
  void goTo(1: string destination);
  GoToStatus getStatus(1: string destination);
  void halt(1: string destination);
  bool isAtLocation (1: string destination);
}
