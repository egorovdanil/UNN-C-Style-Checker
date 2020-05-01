## Telegram chat

https://t.me/joinchat/GRwjklbJOuYz6a2pY8SWeQ

## Установка окружения
### Windows 10
Вам потребуется установить `WSL`. Для этого нужно следовать   [инструкции](https://docs.microsoft.com/ru-ru/windows/wsl/install-win10).      
Обратите внимание что нужно выбрать версию `Ubuntu18`     
После установки запускаем `Ubuntu 18.04 LTS` у вас должен появиться терминал 
### Ubuntu18
Пользователям linux можно приступать к следующему шагу

## Сборка LLVM & CLang
Находясь в терминале установите следующие зависимости:
```
sudo apt install git cmake 
```

Создайте папку для работы с курсом и перейдите в нее
```
mkdir ~/compiler-course && cd ~/compiler-course
```

Далее собираем `LLVM` вместе с `Clang` из исходников
```
git clone https://github.com/llvm/llvm-project
cd llvm-project/llvm/
mkdir build && cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=install \
          -DLLVM_TARGETS_TO_BUILD=X86 \
          -DLLVM_ENABLE_PROJECTS=clang
make -j4
make install
```
Сборка может занять длительное время

## Prebuilt LLVM & Clang
Вы также можете использовать уже собранный llvm & clang, для того что бы получить последнюю версию:
1. Перейдите в папку с задачей.
2. Запустите: `source download-and-install-llvm.sh`
Скрипт скачает последний релиз и положит его в папку `/tmp/`, также выставит переменную `LLVM_DIR`, поэтому вам больше не нужно ее выставлять во время запуска.
3. Перейдите в папку с билдом и запустите:
* `cmake ../`
* `make -j8`

Обратите внимание, что сборка LLVM из исходников потребуется для сдачи дальнейших задач в курсе. Данный `workraround` поможет сдать только текущую задачу.

### Задача
После успешного построения `LLVM & Clang` можно приступать к выполнению задания.

Вернитесь в корневую папку курса:
```
cd ~/compiler-course
```

Для начала сделайте [Fork](http://gearmobile.github.io/git/fork-github/) текущего репозитория к себе в профиль. После этого копия репозитория появится у вас в профиле.

Склонируйте и перейдите в локальную копию:
```
git clone <fork-repo-url>
cd UNN-C-Style-Checker/
```
Создайте отдельную ветку для работы с репозиторием, которая будет содержать имя, фамилию и направление, пример:
```
git checkout -b vasya-pupkin-fiit
```
Собираем проект с заданием
```
mkdir build && cd build
cmake ../ -DLLVM_DIR=~/compiler-course/workspace/llvm-project/llvm/build/install/lib/cmake/llvm
make -j8
```

В задаче нужно реализовать  с помощью интерфейса clang, `tool`, который будет находить в исходном коде программы все приведения типов в стиле си и заменять их на соответсвующий аналог из с++
Пример:
```
double d = 4.5
int i = (int)d; // int i = static_cast<int>(d);
```

Для замены исходного кода программы будем пользоваться классом  [Rewriter](https://clang.llvm.org/doxygen/classclang_1_1Rewriter.html)

Вам нужно будет дописать код в класс `CastCallBack`,  который отвечает за действие, которое нужно совершить при нахождении узла `cStyleCastExpr` в `AST`.  В данном случае мы хотим понять тип преобразования и заменить исходный код с помощью `Rewriter`.
```
class CastCallBack : public MatchFinder::MatchCallback {
public:
	CastCallBack(Rewriter& rewriter) {
		// Your code goes here
	};

	virtual void run(const MatchFinder::MatchResult &Result) {
		// Your code goes here
	}
}
```
Подробнее о том как достать всю необходимую информацию из `MatchFinder::MatchResult` можно посмотреть в файле tool.cpp.

После того как вы реализуете `CastCallBack` , пересоберите проект и запустите.   
```
./c-style-checker ../test/test.cpp --extra-arg=-I/home/<your-root-name>/compiler-course/llvm-project/llvm/build/lib/clang/<version>include/
```

Если вы все реализовали правильно, то вы увидите на экране
```
#include <iostream>
int main() {
	float f;
	int i = static_cast<int>(f);
	return 0;
}
```

### Отправка решения
После того как вы сделали заданиe:
1. Сделайте коммит:
    ```
    git add -u
    git commit -m 'Complete c-style-checker task'
    ```
2. Отправьте ветку с вашим решением на сервер:
    ```
    git push origin HEAD
    ```
3. Перейдите на github и откройте `pull request` из вашей ветки в `master` главного репозитория

Если необходимо что то исправить после создания `pull request` нужно просто сделать изменения и повторить шаги 1 и 2.
Не нужно открывать новый `pull request`


### Полезные ссылки
* [Усложненная реализация данной задачи](https://github.com/llvm-mirror/clang-tools-extra/blob/master/clang-tidy/google/AvoidCStyleCastsCheck.cpp)
* [Матчеры которые есть в clang](https://clang.llvm.org/docs/LibASTMatchersReference.html)
* [Сборка LLVM](https://llvm.org/docs/CMake.html)

