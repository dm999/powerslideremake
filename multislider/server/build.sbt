name := "MultiSlider"

version := "0.8"

scalaVersion := "2.11.8"

exportJars := true

enablePlugins(JavaAppPackaging)

// http://mvnrepository.com/artifact/org.scala-lang/scala-library
libraryDependencies += "org.scala-lang" % "scala-library" % "2.11.8"

// http://mvnrepository.com/artifact/com.typesafe.akka/akka-actor_2.11
libraryDependencies += "com.typesafe.akka" % "akka-actor_2.11" % "2.3.6"

// https://mvnrepository.com/artifact/com.typesafe.akka/akka-remote_2.11
libraryDependencies += "com.typesafe.akka" % "akka-remote_2.11" % "2.3.6"

// http://mvnrepository.com/artifact/org.slf4j/slf4j-api
libraryDependencies += "org.slf4j" % "slf4j-api" % "1.7.16"

// http://mvnrepository.com/artifact/org.apache.logging.log4j/log4j-core
libraryDependencies += "org.apache.logging.log4j" % "log4j-core" % "2.5"

// http://mvnrepository.com/artifact/org.apache.logging.log4j/log4j-api
libraryDependencies += "org.apache.logging.log4j" % "log4j-api" % "2.5"

// http://mvnrepository.com/artifact/log4j/log4j
libraryDependencies += "log4j" % "log4j" % "1.2.17"

// http://mvnrepository.com/artifact/org.slf4j/slf4j-log4j12
libraryDependencies += "org.slf4j" % "slf4j-log4j12" % "1.7.16"

// http://mvnrepository.com/artifact/net.liftweb/lift-json_2.11
libraryDependencies += "net.liftweb" % "lift-json_2.11" % "3.0-M8"

