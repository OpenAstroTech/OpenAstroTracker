package de.andrestefanov.openastrotracker.meade.response

import kotlin.math.abs

data class DEC(
    val degrees: Int,
    val minutes: Int,
    val seconds: Int
) {

    companion object {
        private const val MAX_DEGREES = 90
        private const val MINUTES_PER_DEGREE = 60
        private const val SECONDS_PER_MINUTE = 60
        private const val SECONDS_PER_HOUR = MINUTES_PER_DEGREE * SECONDS_PER_MINUTE
    }

    private constructor(totalSeconds: Int) : this(
        degrees = (totalSeconds / SECONDS_PER_HOUR) % MAX_DEGREES,
        minutes = abs(totalSeconds) % SECONDS_PER_HOUR / SECONDS_PER_MINUTE,
        seconds = abs(totalSeconds) % SECONDS_PER_MINUTE
    )

    fun add(degrees: Int = 0, minutes: Int = 0, seconds: Int = 0) = DEC(
        (degrees + this.degrees) * SECONDS_PER_HOUR +
                (minutes + this.minutes) * SECONDS_PER_MINUTE +
                seconds + this.seconds
    )

}