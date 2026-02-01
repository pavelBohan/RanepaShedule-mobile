import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Simple Random App',
      theme: ThemeData(primarySwatch: Colors.blue),
      home: const RandomNumberScreen(),
    );
  }
}

class RandomNumberScreen extends StatefulWidget {
  const RandomNumberScreen({super.key});

  @override
  State<RandomNumberScreen> createState() => _RandomNumberScreenState();
}

class _RandomNumberScreenState extends State<RandomNumberScreen> {
  String? _randomNumber;
  bool _loading = false;

  Future<void> getRandomNumber() async {
    setState(() {
      _loading = true;
      _randomNumber = null;
    });

    try {
      // Отправляем команду боту
      final response = await http.post(
        Uri.parse('https://api.telegram.org/bot8470518497:AAEbUQP9efjCSuRvZjbt_2CA99LQe80VpNk/sendMessage'),
        headers: {'Content-Type': 'application/json'},
        body: jsonEncode({
          'chat_id': 5282212117,
          'text': '/get_random',
          'disable_notification': true,
        }),
      );

      if (response.statusCode == 200) {
        // Ждём 3 секунды пока бот обработает
        await Future.delayed(const Duration(seconds: 3));
        
        // Получаем последнее сообщение от бота
        final messagesResponse = await http.get(
          Uri.parse('https://api.telegram.org/bot8470518497:AAEbUQP9efjCSuRvZjbt_2CA99LQe80VpNk/getUpdates'),
        );
        
        if (messagesResponse.statusCode == 200) {
          final data = jsonDecode(messagesResponse.body);
          if (data['ok'] && data['result'] is List) {
            final messages = data['result'] as List;
            // Находим последнее сообщение от бота
            for (var i = messages.length - 1; i >= 0; i--) {
              final msg = messages[i];
              if (msg['message']?['from']?['is_bot'] == true) {
                final text = msg['message']['text'] as String;
                if (text.contains('🎲 Случайное число')) {
                  setState(() {
                    _randomNumber = text;
                    _loading = false;
                  });
                  return;
                }
              }
            }
          }
        }
      }
      
      setState(() {
        _randomNumber = '❌ Не удалось получить данные';
        _loading = false;
      });
    } catch (e) {
      setState(() {
        _randomNumber = '❌ Ошибка: $e';
        _loading = false;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Случайное число')),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            if (_loading) const CircularProgressIndicator(),
            if (_randomNumber != null) Text(_randomNumber!),
            const SizedBox(height: 20),
            ElevatedButton(
              onPressed: getRandomNumber,
              child: const Text('Получить случайное число'),
            ),
          ],
        ),
      ),
    );
  }
}