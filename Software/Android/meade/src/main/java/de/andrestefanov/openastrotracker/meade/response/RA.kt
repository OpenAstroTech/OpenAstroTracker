package de.andrestefanov.openastrotracker.meade.response

import kotlin.math.abs

data class RA(
    val hours: Int,
    val minutes: Int,
    val seconds: Int
) {

    companion object {
        private const val MAX_HOURS = 24
        private const val MINUTES_PER_HOUR = 60
        private const val SECONDS_PER_MINUTE = 60
        private const val SECONDS_PER_HOUR = MINUTES_PER_HOUR * SECONDS_PER_MINUTE
    }

    private constructor(totalSeconds: Int) : this(
        hours = (totalSeconds / SECONDS_PER_HOUR) % MAX_HOURS,
        minutes = abs(totalSeconds) % SECONDS_PER_HOUR / SECONDS_PER_MINUTE,
        seconds = abs(totalSeconds) % SECONDS_PER_MINUTE
    )

    fun add(hours: Int = 0, minutes: Int = 0, seconds: Int = 0) = RA(
        (hours + this.hours) * SECONDS_PER_HOUR +
                (minutes + this.minutes) * SECONDS_PER_MINUTE +
                seconds + this.seconds
    )

}