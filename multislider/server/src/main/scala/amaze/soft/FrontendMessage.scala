package amaze.soft

/**
 * Created by Alexey on 21.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

object FrontendMessage {
  trait JsonMessage

  /**
   * Session message wrapper
   * @param roomName name of destination room
   * @param data serialized message
   */
  case class Envelop(roomName: String, data: String) extends JsonMessage

  /**
   * Client sends the message in order to create a new room
   * @param playerName player name who creates the room, he will be host
   * @param roomName the new room's name
   * @param description room description
   * @param playersLimit max players amount
   * @param playersReserved amount of reserved players slots
   * @param userParam arbitrary user data
   */
  case class CreateRoom(playerName: String, roomName: String, description: String, playersLimit: Int, playersReserved: Int, userParam: String) extends JsonMessage

  /**
   * Change parameters of the room
   */
  case class Reconfigure(playersLimit: Int, playersReserved: Int) extends JsonMessage

  /**
   * Close a previously created room
   */
  case class CloseRoom() extends JsonMessage

  /**
   * Get a list of all created rooms
   */
  case class GetRooms() extends JsonMessage

  /**
   * Join a room
   * @param playerName player name
   * @param roomName room name to join
   */
  case class JoinRoom(playerName: String, roomName: String) extends JsonMessage

  /**
   * Leave currently joined room
   */
  case class LeaveRoom() extends JsonMessage

  /**
   * Eject player from my room
   * Can be send only by host
   */
  case class EjectPlayer(playerName: String) extends JsonMessage

  /**
   * Is sent by any user when he changed some settings
   */
  case class Update(room: RoomInfo, data: String, sender: String, toSelf: Boolean, flags: Int) extends JsonMessage

  /**
   * Message between players without any caching
   */
  case class Message(sender: String, data: String, toSelf: Boolean) extends JsonMessage

  /**
   * Start game session for all current players
   */
  case class StartSession(data: String) extends JsonMessage

  //---------------------------------------------------------
  // Outgoing messages

  /**
   * Greeting message with server information
   */
  case class Greeting(serverName: String, verMajor: Int, verMinor: Int, verRevision: Int) extends JsonMessage

  /**
   * Sent when session is created
   */
  case class SessionStarted(ip: String, port: Int, name: String, id: Int, data: String) extends JsonMessage

  /**
   * Sent when client was ejected
   */
  case class Ejected(flags: Int) extends JsonMessage

  /**
   * Changed room parameters successfully
   */
  case class ReconfigureSucc(room: RoomInfo) extends JsonMessage

  /**
   * Failed to change room parameters
   */
  case class ReconfigureSuck() extends JsonMessage
}
