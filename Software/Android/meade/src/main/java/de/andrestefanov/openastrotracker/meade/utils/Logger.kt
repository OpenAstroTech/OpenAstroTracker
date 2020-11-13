package de.andrestefanov.openastrotracker.meade.utils

var MEADE_LOGGER: Logger? = null

internal fun logv(tag: String, message: String) =
    MEADE_LOGGER?.logv(tag, message)

internal fun loge(tag: String, message: String, exception: Exception? = null) =
    MEADE_LOGGER?.loge(tag, message, exception)

interface Logger {
    fun logv(tag: String, message: String)
    fun loge(tag: String, message: String, exception: Exception? = null)
}