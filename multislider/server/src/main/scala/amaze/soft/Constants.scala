package amaze.soft

import scala.concurrent.duration._

/**
 * Created by Alexey on 21.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */
object Constants {

  val DEFAULT_NAME = "MultiSlider"

  val VERSION_MAJOR = 0
  val VERSION_MINOR = 8
  val VERSION_REVISION = 0

  val RESPONCE_GREETINGS = "HALO"
  val RESPONSE_SUCC = "SUCC"
  val RESPONSE_SUCK = "SUCK"
  val RESPONSE_ROOM_IS_FULL = "FULL"
  val RESPONSE_NAME_EXISTS  = "NAME"
  val RESPONSE_ROOM_EXISTS  = "ROOM"
  val RESPONSE_SUCK_IS_FULL = "SERVER_IS_FULL"
  val MESSAGE_EJECTED = "EJECTED"

  val FUTURE_TIMEOUT = 30 seconds

  val FRONTEND_NAME = "MultiSlider:Frontend"
  val BACKEND_NAME = "MultiSlider:Backend"
  val DOG_API_NAME = "MultiSlider:DogApi"

  val MESSAGE_ENCODING = "UTF-8"

  // Flags for Update and Left events
  val FLAG_EJECTED        = 1
  val FLAG_JOINED         = 1 << 1
  val FLAG_LEFT           = 1 << 2
  val FLAG_NEW_HOST       = 1 << 3
  val FLAG_CLOSED_BY_HOST = 1 << 4
}
