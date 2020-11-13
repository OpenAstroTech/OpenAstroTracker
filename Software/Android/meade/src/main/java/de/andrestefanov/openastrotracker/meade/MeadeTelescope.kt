package de.andrestefanov.openastrotracker.meade

import de.andrestefanov.openastrotracker.meade.network.Connection
import de.andrestefanov.openastrotracker.meade.response.DEC
import de.andrestefanov.openastrotracker.meade.response.RA
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.flow
import kotlinx.coroutines.flow.flowOn

/**
 * Meade LX200 classic protocol client implementation with some extensions.
 */
@Suppress("MemberVisibilityCanBePrivate")
data class MeadeTelescope(val name: String, val connection: Connection) {

    // Initialize family

    /**
     * Put the Arduino in Serial Control Mode.
     */
    fun initializeScope() = command(":I#")

    // Sync control family

    /**
     * Synchronize Declination and Right Ascension. The values to be used have to be previously set
     * by [setRA] and [setDEC].
     */
    fun syncScope() = commandWithResponse(
        command = ":CM#",
        regex = Regex("""(NONE)"""),
        factory = { true }
    )

    // Distance family

    fun getSlewingStatus() = commandWithResponse(
        command = ":D#",
        regex = Regex("""(\s?)""")
    ) {
        it.first().isNotEmpty()
    }

    // GPS family

    // GET family

    fun getDEC() = commandWithResponse(
        command = ":GD#",
        regex = Regex("""([-+]?\d{2})\*(\d{2})'(\d{2})""")
    ) {
        val (degrees, minutes, seconds) = it.map(String::toInt)
        DEC(degrees, minutes, seconds)
    }

    fun getRA() = commandWithResponse(
        command = ":GR#",
        regex = Regex("""(\d{2}):(\d{2}):(\d{2})""")
    ) {
        val (hours, minutes, seconds) = it.map(String::toInt)
        RA(hours, minutes, seconds)
    }

    // SET family

    fun setDEC(dec: DEC) = commandWithStatus(
        command = with(dec) { ":Sd$degrees*$minutes:$seconds#" }
    )

    fun setRA(ra: RA) = commandWithStatus(
        command = with(ra) { ":Sd$hours:$minutes:$seconds#" }
    )

    // SET family extensions

    // Rate control family

    // Movement family

    // Movement extensions

    // Home family

    // Quit movement family

    // Extra OAT family

    fun setTrackingMode(track: Boolean) = commandWithStatus(
        command = ":MT${if (track) 1 else 0}#"
    )


    private fun command(command: String) = flow {
        emit(connection.command(command))
    }.flowOn(Dispatchers.IO)

    private fun <T> commandWithResponse(
        command: String,
        regex: Regex,
        factory: (List<String>) -> T
    ) = flow {
        val response = connection.commandWithResponse(command)
        regex.find(response.removeSuffix("#"))?.run {
            emit(factory(destructured.toList()))
        } ?: throw IllegalArgumentException(
            "Failed to deserialize response $response with regex $regex"
        )
    }.flowOn(Dispatchers.IO)

    private fun commandWithStatus(command: String) = commandWithResponse(
        command = command,
        regex = COMMAND_STATUS_REGEX,
        factory = COMMAND_STATUS_FACTORY
    )

    companion object {
        private val COMMAND_STATUS_REGEX = Regex("""(\d)""")
        private val COMMAND_STATUS_FACTORY: (List<String>) -> Boolean = {
            when (it.first()) {
                "0" -> false
                "1" -> true
                else -> throw IllegalArgumentException("Failed to map String \"$this\" to Boolean")
            }
        }
    }
}