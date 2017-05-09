package amaze.soft

import akka.actor.Actor
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory

/**
 * Created by Alexey on 08.08.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 *
 * Api for a watchdog
 */

object DogApi
{
  trait JsonMessage
  // Incoming
  case class Hello() extends JsonMessage

  // Outcoming
  case class Identity(name: String, verMajor: Int, verMinor: Int)

  //-------------------------
  val ms_logger = LoggerFactory.getLogger(DogApi.getClass.getName)
}

class DogApi extends Actor  {
  import DogApi._

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(classOf[DogApi.Hello])))

  def receive = {
    case message: String =>
      ms_logger.info("Got message: " + message.toString)
      try {
        json.parse(message).extract[DogApi.JsonMessage] match {
          case Hello() =>
            sender() ! json.Serialization.write(Identity(Constants.DEFAULT_NAME, Constants.VERSION_MAJOR, Constants.VERSION_MINOR))
        }
      } catch  {
        case err: Exception =>
          ms_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
      }

    case _: Any =>
      ms_logger.warn("Unknown message!")
  }
}
