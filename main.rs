mod front_end;
use front_end::lexer::lexer::Lexer;

fn main() {
    let source = String::from("let x = 5");
    let lexer = Lexer::new(source);

    println!("{:?}", lexer.lex());
}
