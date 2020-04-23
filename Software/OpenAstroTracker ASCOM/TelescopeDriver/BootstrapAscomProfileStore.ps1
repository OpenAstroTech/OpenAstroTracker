<#
This script initializes a bare minimum set of registry entries required for ASCOM.Utilities.Profile to work
without throwing any exceptions. When building on a build server, or on a computer without the ASCOM Platform installed,
it may be useful to execute this script as a build step prior to running any unit tests, or calling any code that relies on
ASCOM.Utilities.Profile. The alternative is to install the ASCOM Platform on the build agent.

NOTE: This script equires elevated permissions because it creates registry keys in the LocalMachine hive.
#>

$wow = Test-Path HKLM:\SOFTWARE\Wow6432Node
if ($wow)
    {
    $root = "HKLM:\SOFTWARE\Wow6432Node"
    }
else
    {
    $root = "HKLM:\SOFTWARE"
    }
$ascomRoot = $root + "\ASCOM"

if (Test-Path $ascomRoot) 
    {
    <# Don't upset an already-existing ASCOM registry #>
    exit
    }

<# Create the ASCOM root key and set it's ACL to allow all users read/write access #>
New-Item -Path $root -Name ASCOM –Force
$ascomAcl = Get-Acl $ascomRoot
$aclRule = New-Object System.Security.AccessControl.RegistryAccessRule ("Users","FullControl","Allow")
$ascomAcl.SetAccessRule($aclRule)
$ascomAcl | Set-Acl -Path $ascomRoot

<# Now create the bare minimum keys required so that ASCOM.Utilities.Profile doesn't crash and burn #>
New-ItemProperty -Path $ascomRoot -Name PlatformVersion -Value "6.1" -PropertyType String –Force
New-ItemProperty -Path $ascomRoot -Name SerTraceFile -Value "C:\SerialTraceAuto.txt" -PropertyType String –Force
