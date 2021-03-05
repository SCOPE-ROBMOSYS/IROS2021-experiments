/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

 /**
 * Enumerates the states every node can be in after execution during a particular
 * time step:
 * - "SKILL_SUCCESS" indicates that the node has completed running during this time step;
 * - "SKILL_FAILURE" indicates that the node has determined it will not be able to complete
 *   its task;
 * - "SKILL_RUNNING" indicates that the node has successfully moved forward during this
 *   time step, but the task is not yet complete;
 * - "SKILL_IDLE" indicates that the node hasn't run yet.
 */
enum SkillAck {SKILL_IDLE, SKILL_RUNNING, SKILL_SUCCESS, SKILL_FAILURE, SKILL_STARTED, SKILL_STOPPED}

service Skill_request {

    /**
     * request_ack  Get the ack of the skill.
     *
     * return              The enum indicating the ack of the skill..
     */
    SkillAck request_ack()

    /**
     * request_stop  Send a cmd_start skill.
     *
     * return               void.
     */
    void send_start();

    /**
     * send_stop  Send a CMD_STOP request to the skill.
     *
     * return              void.
     */
     void send_stop();
}
