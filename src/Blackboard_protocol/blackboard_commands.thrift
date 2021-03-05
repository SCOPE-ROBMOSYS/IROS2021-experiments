/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/
enum requestEvaluation{
  UNKNOWN,
  VALID,
  INVALID_KEY,
  UNAUTHORIZED_CLIENT
}

struct DataInt32 {
  1: i32 dataValue;
  2: requestEvaluation reqEval = requestEvaluation.UNKNOWN; 
}

struct DataFloat64 {
  1: double dataValue;
  2: requestEvaluation reqEval = requestEvaluation.UNKNOWN; 
}

struct DataString {
  1: string dataValue;
  2: requestEvaluation reqEval = requestEvaluation.UNKNOWN; 
}

struct DataBool {
  1: bool dataValue;
  2: requestEvaluation reqEval = requestEvaluation.UNKNOWN; 
}

service Blackboard {
  DataBool lock(1: string skillID, 2: string target)
  DataBool unlock(1: string skillID, 2: string target)
  DataInt32 readDataInt32(1: string skillID, 2: string target)
  DataString readDataString(1: string skillID, 2: string target)
  DataFloat64 readDataFloat64(1: string skillID, 2: string target)
  DataBool writeDataInt32(1: string skillID, 2: string target, 3: i32 datum)
  DataBool writeDataString(1: string skillID, 2: string target, 3: string datum)
  DataBool writeDataFloat64(1: string skillID, 2: string target, 3: double datum)
  void clearData(1: string target)
  void clearAll()
  void resetData()
  list<string> listAll()
}
