namespace App.WindowsService;

public sealed class AppGuardService
{
    public string GetText()
    {
        AppGuardText text = _text.ElementAt(
            Random.Shared.Next(_text.Count));

        return $"{text.Text1}{Environment.NewLine}{text.Text2}";
    }

    private readonly HashSet<AppGuardText> _text = new()
    {
        new AppGuardText("What's the best thing about a Boolean?", "Even if you're wrong, you're only off by a bit."),
        new AppGuardText("What's the object-oriented way to become wealthy?", "Inheritance"),
        new AppGuardText("Why did the programmer quit their job?", "Because they didn't get arrays."),
        new AppGuardText("Why do programmers always mix up Halloween and Christmas?", "Because Oct 31 == Dec 25"),
        new AppGuardText("How many programmers does it take to change a lightbulb?", "None that's a hardware problem"),
        new AppGuardText("If you put a million monkeys at a million keyboards, one of them will eventually write a Java program", "the rest of them will write Perl"),
        new AppGuardText("['hip', 'hip']", "(hip hip array)"),
        new AppGuardText("To understand what recursion is...", "You must first understand what recursion is"),
        new AppGuardText("There are 10 types of people in this world...", "Those who understand binary and those who don't"),
        new AppGuardText("Which song would an exception sing?", "Can't catch me - Avicii"),
        new AppGuardText("Why do Java programmers wear glasses?", "Because they don't C#"),
        new AppGuardText("How do you check if a webpage is HTML5?", "Try it out on Internet Explorer"),
        new AppGuardText("A user interface is like a text.", "If you have to explain it then it is not that good."),
        new AppGuardText("I was gonna tell you a text about UDP...", "...but you might not get it."),
        new AppGuardText("The punchline often arrives before the set-up.", "Do you know the problem with UDP jokes?"),
        new AppGuardText("Why do C# and Java developers keep breaking their keyboards?", "Because they use a strongly typed language."),
        new AppGuardText("Knock-knock.", "A race condition. Who is there?"),
        new AppGuardText("What's the best part about TCP jokes?", "I get to keep telling them until you get them."),
        new AppGuardText("A programmer puts two glasses on their bedside table before going to sleep.", "A full one, in case they gets thirsty, and an empty one, in case they don’t."),
        new AppGuardText("There are 10 kinds of people in this world.", "Those who understand binary, those who don't, and those who weren't expecting a base 3 text."),
        new AppGuardText("What did the router say to the doctor?", "It hurts when IP."),
        new AppGuardText("An IPv6 packet is walking out of the house.", "He goes nowhere."),
        new AppGuardText("3 SQL statements walk into a NoSQL bar. Soon, they walk out", "They couldn't find a table.")
    };
}

readonly record struct AppGuardText(string Text1, string Text2);