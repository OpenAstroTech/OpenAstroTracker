package de.andrestefanov.openastrotracker.android

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.livedata.observeAsState
import androidx.compose.ui.platform.setContent
import androidx.compose.ui.viewinterop.viewModel
import de.andrestefanov.openastrotracker.android.ui.OpenAstroTrackerTheme
import de.andrestefanov.openastrotracker.android.ui.compose.Root
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeState
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeViewModel
import de.andrestefanov.openastrotracker.meade.MeadeTelescope
import de.andrestefanov.openastrotracker.meade.network.Connection

private const val TAG = "MainActivity"

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            OpenAstroTrackerTheme {
                // A surface container using the 'background' color from the theme
                Surface(color = MaterialTheme.colors.background) {
                    Root()
                }
            }
        }
    }
}

@Composable
fun TelescopeStateView(
    protocol: String,
    address: String,
    telescopeViewModel: TelescopeViewModel = viewModel()
) {

    val state: TelescopeState? by telescopeViewModel.data(
        MeadeTelescope(
            "TODO",
            Connection.from(protocol, address)
        )
    ).observeAsState()

    Text(text = state.toString())
}