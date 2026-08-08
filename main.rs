mod front_end;

fn main() {
    let source = "5 + 1";
    let tokens = front_end::lexer::lexer::lex(source);

    println!("{:?}", tokens);
}
