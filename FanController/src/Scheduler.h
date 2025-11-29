// Button init (if enabled in JSON config)
if (g_config.buttonEnabled)
{
    static InputButton btn(g_config.buttonPin, true);
    g_button = &btn;
    g_button->begin();
}

// Pot init (if enabled)
if (g_config.potEnabled)
{
    static InputPot pot(g_config.potPin);
    g_pot = &pot;
    g_pot->begin();
}
