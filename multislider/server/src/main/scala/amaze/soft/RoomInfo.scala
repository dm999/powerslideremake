package amaze.soft

/**
 * Created by Alexey on 11.07.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

/**
 * Room descriptor for clients
 * @param name Name of the room
 * @param host Player name of the host
 * @param description Room description
 * @param playersLimit Maximum number of players in the room
 * @param playersReserved Amount of reserved players slots
 * @param playersNumber Current number of players, from 0 till playersLimit - playersReserved
 * @param players List of player names
 * @param userParam Arbitrary user parameter of the room
 */
case class RoomInfo(name: String, var host: String,
                    description: String,
                    playersLimit: Int, playersReserved : Int, var playersNumber: Int,
                    var players: List[String],
                    userParam: String)
{
  def noPlayers = RoomInfo(name, host, description, playersLimit, playersReserved, playersNumber, null, userParam)
}
