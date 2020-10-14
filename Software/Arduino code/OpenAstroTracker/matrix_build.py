Import("env")

import itertools

boards = ["mega2560", "esp32", "uno"]
boards = map(lambda x: "-e {}".format(x), boards)
displays = ["headless", "lcd"]
displays = map(lambda x: "-e {}".format(x), displays)

values = [
    boards,
    displays
]

configs = list(itertools.product(*values))

commands = map(lambda x: "platformio run {}".format(x), boards)

print(*commands)

env.AddCustomTarget(
    name="matrix",
    dependencies=None,
    actions=commands,
    title=None,
    description=None,
    always_build=True
)