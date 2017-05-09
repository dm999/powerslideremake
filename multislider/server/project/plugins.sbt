logLevel := Level.Warn

resolvers ++= Seq(
  "sonatype-releases" at "https://oss.sonatype.org/content/repositories/releases/",
  "bintray-sbt-plugin-releases" at "http://dl.bintray.com/content/sbt/sbt-plugin-releases",
  "Typesafe repository" at "http://repo.typesafe.com/typesafe/releases/"
)

// to package applications
addSbtPlugin("com.typesafe.sbt" %% "sbt-native-packager" % "1.1.0-M3")

// to format scala source code
addSbtPlugin("com.typesafe.sbt" % "sbt-scalariform" % "1.3.0")

// enable updating file headers eg. for copyright
addSbtPlugin("de.heikoseeberger" % "sbt-header"      % "1.5.1")