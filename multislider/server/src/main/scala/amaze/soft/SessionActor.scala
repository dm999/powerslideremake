package amaze.soft

import java.net.InetSocketAddress

import akka.actor.{Actor, ActorRef}
import akka.io.Udp
import akka.util.ByteString
import amaze.soft.Backend.CloseSession
import amaze.soft.BackendMessage._
import akka.actor.Cancellable
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory
import scala.concurrent.duration._
import scala.language.postfixOps

object SessionActor
{
  val KEEP_ALIVE_INTERVAL = 20 second
  val KEEP_ALIVE_LIMIT = 10 * KEEP_ALIVE_INTERVAL
  val WEAK_CONNECTION_TIMEOUT = 20 second

  val ACK_HISTORY_SIZE = 32
  val IDX_WRAP_MODULE = 1024

  /**
   * Client data envelop with back address
   */
  case class SignedEnvelop(socket: ActorRef, address: InetSocketAddress, data: String)

  /**
   * Dynamic information for each player
   */
  case class PlayerStatistics(address: InetSocketAddress, var updateTimestamp: Long, var privateData: String, var lastPingTime: Long, var weakConnection: Boolean)

  case class MessagesSequenceInfo(localSeqIdx: Int, remoteSeqIdx: Int, remoteIdxBits: Long)

  /**
   * Data broadcasted for each player
   */
  case class PlayerData(name: String, data: String, timestamp: Long, weak: Boolean, alive: Boolean)

  object State extends Enumeration
  {
    type State = Value
    val Waiting, Running, Zombie = Value
  }
}

/**
 * Created by Alexey on 22.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */
class SessionActor(m_id: Int, m_name: String, players: List[String]) extends Actor{
  import SessionActor.State._
  import SessionActor._

  private val m_logger = LoggerFactory.getLogger(SessionActor.getClass.getName)
  m_logger.info("Session \"" + m_name + "\" is created!")

  val m_stats: scala.collection.mutable.HashMap[String, PlayerStatistics] =
    scala.collection.mutable.HashMap(players.map(name => {name -> null}).toSeq:_*)

  var m_shared_stats = PlayerStatistics(null, 0, "", 0, weakConnection=false)

  var m_seqStats: scala.collection.mutable.HashMap[String, MessagesSequenceInfo] = scala.collection.mutable.HashMap()

  var m_state = Waiting

  var m_lastPing = System.currentTimeMillis()

  var m_keepAliveTask: Cancellable = null

  m_logger.info(players.toString())

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(
    classOf[Ready], classOf[Start], classOf[Update], classOf[SessionState], classOf[Quit], classOf[RequestSync],
    classOf[Sync], classOf[KeepAlive], classOf[Ack], classOf[ClockSync], classOf[Message])))

  private def gatherSessionData(): String = {
    json.Serialization write (m_stats.map{case (name, stats) =>
        if(stats != null)
          PlayerData(name, stats.privateData, stats.updateTimestamp, weak=stats.weakConnection, alive=true)
        else
          PlayerData(name, "", 0, weak=false, alive=false)
      } toList )
  }

  private def checkConnection() = {
    val currTime = System.currentTimeMillis()
    m_stats foreach { case (name, player) =>
      if(player != null && (currTime > player.lastPingTime)) {
        if(currTime - player.lastPingTime > KEEP_ALIVE_LIMIT.toMillis) {
          m_logger.info("Connection with the player \"" + name + "\" is lost!")
          quitImpl(name)
        }
        else {
          player.weakConnection = currTime - player.lastPingTime > WEAK_CONNECTION_TIMEOUT.toMillis
          m_stats.update(name, player)
        }
      }
    }
  }

  private def quitImpl(playerName: String) = {
    val playerStats = m_stats.get(playerName)
    if (playerStats.isDefined && playerStats.get != null) {
      m_stats.update(playerName, null)
      m_logger.info("\"" + playerName + "\" quits session \"" + m_name + "\"")
      if (m_stats.values.forall(_ == null)) {
        m_logger.info("All players left session \"" + m_name + "\". Exiting.")
        m_keepAliveTask.cancel()
        Depot.backend ! CloseSession(m_id)
        context stop self
      }
    }
  }

  private def checkAcknowlegment(ackIdx: Int, player: String): Boolean = {
    val opt = m_seqStats.get(player)
    if(opt.isDefined) {
      val info = opt.get
      if(info.remoteSeqIdx == ackIdx){
        return false
      }
      if((ackIdx > info.remoteSeqIdx) && (ackIdx - info.remoteSeqIdx < ACK_HISTORY_SIZE)){
         m_seqStats.update(player, MessagesSequenceInfo(info.localSeqIdx, ackIdx, info.remoteIdxBits << (ackIdx - info.remoteSeqIdx)))
         return true
      }
      if((IDX_WRAP_MODULE + ackIdx > info.remoteSeqIdx) && (IDX_WRAP_MODULE + ackIdx - info.remoteSeqIdx < ACK_HISTORY_SIZE)){
        m_seqStats.update(player, MessagesSequenceInfo(info.localSeqIdx, ackIdx, info.remoteIdxBits << (IDX_WRAP_MODULE + ackIdx - info.remoteSeqIdx)))
        return true
      }
      if((ackIdx > info.remoteSeqIdx) && (ackIdx - info.remoteSeqIdx < ACK_HISTORY_SIZE)) {
        val ackBit = 0x1 << (ackIdx - info.remoteSeqIdx - 1)
        if (0 == (info.remoteIdxBits & ackBit)) {
          m_seqStats.update(player, MessagesSequenceInfo(info.localSeqIdx, info.remoteSeqIdx, info.remoteIdxBits | ackBit))
          return true
        }
      }
    }
    false
  }

  override def receive = {
    case SignedEnvelop(socket, senderAddress, rawJson) =>
      //m_logger.info("Got an Envelop message")
      try {
        val msg = json.parse(rawJson).extract[JsonMessage]
        //m_logger.info(msg.toString)
        m_state match {
          //--------------------------------------------------------
          case Waiting =>
            msg match {
              case Ready(playerName) =>
                m_logger.info("Got a Ready message")
                if (m_stats.contains(playerName)) {
                  m_stats.update(playerName, PlayerStatistics(senderAddress, 0, "", System.currentTimeMillis(), weakConnection=false))
                  m_seqStats.update(playerName, MessagesSequenceInfo(1, 0, 0))
                  // if no nulls then all players have joined
                  if(!m_stats.values.exists(_ == null)){
                    m_state = Running
                    m_stats.values.foreach({stat => socket ! Udp.Send(ByteString(json.Serialization.write(Start())), stat.address)})
                    m_keepAliveTask = Depot.actorsSystem.scheduler.schedule(KEEP_ALIVE_INTERVAL, KEEP_ALIVE_INTERVAL) {
                      checkConnection()
                      m_stats.values.foreach({player => if(player != null) socket ! Udp.Send(ByteString(json.Serialization.write(KeepAlive(null, System.currentTimeMillis()))), player.address)})
                    } (Depot.actorsSystem.dispatcher)
                  } else {
                    socket ! Udp.Send(ByteString(Constants.RESPONSE_SUCC), senderAddress)
                  }
                }
                else {
                  socket ! Udp.Send(ByteString(Constants.RESPONSE_SUCK), senderAddress)
                }

              case KeepAlive(playerName, _) =>
                val player = m_stats.get(playerName)
                if(player.isDefined){
                  player.get.lastPingTime = System.currentTimeMillis()
                }

              case _ =>
                m_logger.error("[Waiting] Unexpected message")
            }
          //--------------------------------------------------------
          case Running =>
              msg match {
                case Ready(playerName) =>
                  () // Ignore

                case Update(playerName, timestamp, forceBroadcast, privateData, sharedData) =>
                  //m_logger.info("Got a Update message")
                  val playerStats = m_stats.get(playerName)
                  if(playerStats.isDefined){
                    var player = playerStats.get
                    if(player != null && player.updateTimestamp < timestamp) {
                      player.updateTimestamp = timestamp
                      player.privateData = privateData
                      player.lastPingTime = System.currentTimeMillis()
                      m_stats.update(playerName, player)
                      if (sharedData != null && sharedData.nonEmpty && m_shared_stats.updateTimestamp < timestamp) {
                        m_shared_stats.updateTimestamp = timestamp
                        m_shared_stats.privateData = sharedData
                      }
                      if(forceBroadcast) {
                        m_stats.values.foreach( stat => socket ! Udp.Send(ByteString(json.Serialization.write(SessionState(gatherSessionData(), m_shared_stats.privateData, m_shared_stats.updateTimestamp))), stat.address))
                      }else {
                        socket ! Udp.Send(ByteString(json.Serialization.write(SessionState(gatherSessionData(), m_shared_stats.privateData, m_shared_stats.updateTimestamp))), senderAddress)
                      }
                    } else {
                      m_logger.info("[Running] Got outdated Update message")
                    }
                  }

                case Message(sender, _) =>
                  m_logger.info("Got a Message message")
                  for((name, player) <- m_stats if name != sender) {
                    socket ! Udp.Send(ByteString(rawJson), player.address)
                  }

                case KeepAlive(playerName, _) =>
                  val player = m_stats.get(playerName)
                  if(player.isDefined){
                    player.get.lastPingTime = System.currentTimeMillis()
                  }

                case RequestSync(playerName, delay, syncId, seqIdx) =>
                  m_logger.info("Got a RequestSync message")
                  val scheduleSync = (msgIdx: Int) => {
                    Depot.actorsSystem.scheduler.scheduleOnce(delay milliseconds) {
                      for(player <- m_stats.values if player != null) {
                        socket ! Udp.Send(ByteString(json.Serialization.write(Sync(syncId, msgIdx))), player.address)
                      }
                    }(Depot.actorsSystem.dispatcher)
                  }
                  if (seqIdx < IDX_WRAP_MODULE) {
                    if (checkAcknowlegment(seqIdx, playerName)) {
                      // Reliable
                      val info = m_seqStats.get(playerName).get
                      m_seqStats.update(playerName, MessagesSequenceInfo(if (info.localSeqIdx < IDX_WRAP_MODULE - 1) info.localSeqIdx + 1 else 0, info.remoteSeqIdx, info.remoteIdxBits))
                      scheduleSync(info.localSeqIdx)
                      socket ! Udp.Send(ByteString(json.Serialization.write(Ack(seqIdx))), senderAddress)
                    }
                  }else {
                    // Simple
                    scheduleSync(IDX_WRAP_MODULE + 1)
                  }

                case ClockSync(id, requestTime, _) =>
                  m_logger.info("Got a ClockSync message")
                  socket ! Udp.Send(ByteString(json.Serialization.write(ClockSync(id, requestTime, System.currentTimeMillis()))), senderAddress)

                case Quit(playerName) =>
                  m_logger.info("Got a Quit message")
                  quitImpl(playerName)
                  for(stat <- m_stats.values if (stat != null) && (stat.address != senderAddress)) {
                    socket ! Udp.Send(ByteString(json.Serialization.write(Quit(playerName))), stat.address)
                  }

                case _ => m_logger.error("[Running] Unexpected message")
              }
          //--------------------------------------------------------
          case Zombie =>
            msg match {
              case _ => m_logger.error("[Running] Unexpected message")
            }
        }
      }
      catch {
        case err: Exception =>
          m_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
          socket ! Udp.Send(ByteString(Constants.RESPONSE_SUCK), senderAddress)
      }

    case _ =>
      m_logger.error("SessionActor: Unknown message!")
  }
}
