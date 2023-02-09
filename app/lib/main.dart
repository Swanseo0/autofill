import 'package:flutter/material.dart';

void main() => runApp(const MyApp());

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    const appTitle = 'Form Styling Demo';
    return MaterialApp(
      title: appTitle,
      home: Scaffold(
        appBar: AppBar(
          title: const Text(appTitle),
        ),
        body: const MyCustomForm(),
      ),
    );
  }
}

class MyCustomForm extends StatelessWidget {
  const MyCustomForm({super.key});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: <Widget>[
        const Padding(
          padding: EdgeInsets.symmetric(horizontal: 8, vertical: 16),
          child: TextField(
            enableSuggestions: true,
            autofillHints: [AutofillHints.username, AutofillHints.username],
            decoration: InputDecoration(
              border: OutlineInputBorder(),
              hintText: 'Enter a search username',
            ),
          ),
        ),
        Padding(
          padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 16),
          child: TextFormField(
            enableSuggestions: true,
            decoration: const InputDecoration(
              border: UnderlineInputBorder(),
              labelText: 'Enter your username',
            ),
          ),
        ),
      ],
    );
  }
}

class MyStatefulWidget extends StatefulWidget {
  const MyStatefulWidget({super.key});

  @override
  State<MyStatefulWidget> createState() => _MyStatefulWidgetState();
}

class _MyStatefulWidgetState extends State<MyStatefulWidget> {
  bool isSameAddress = true;
  final TextEditingController id = TextEditingController();
  final TextEditingController shippingAddress2 = TextEditingController();
  final TextEditingController billingAddress1 = TextEditingController();
  final TextEditingController billingAddress2 = TextEditingController();

  final TextEditingController creditCardNumber = TextEditingController();
  final TextEditingController creditCardSecurityCode = TextEditingController();

  final TextEditingController phoneNumber = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return ListView(
      children: <Widget>[
        const Text('Shipping address'),
        // The address fields are grouped together as some platforms are
        // capable of autofilling all of these fields in one go.
        AutofillGroup(
          child: Column(
            children: <Widget>[
              TextField(
                controller: id,
                autofillHints: const <String>[AutofillHints.username],
              ),
              TextField(
                controller: shippingAddress2,
                autofillHints: const <String>[AutofillHints.streetAddressLine2],
              ),
            ],
          ),
        ),
        const Text('Billing address'),
        Checkbox(
          value: isSameAddress,
          onChanged: (bool? newValue) {
            if (newValue != null) {
              setState(() {
                isSameAddress = newValue;
              });
            }
          },
        ),
        // Again the address fields are grouped together for the same reason.
        if (!isSameAddress)
          AutofillGroup(
            child: Column(
              children: <Widget>[
                TextField(
                  controller: billingAddress1,
                  autofillHints: const <String>[
                    AutofillHints.streetAddressLine1,
                  ],
                ),
                TextField(
                  controller: billingAddress2,
                  autofillHints: const <String>[
                    AutofillHints.streetAddressLine2,
                  ],
                ),
              ],
            ),
          ),
        const Text('Credit Card Information'),
        // The credit card number and the security code are grouped together
        // as some platforms are capable of autofilling both fields.
        AutofillGroup(
          child: Column(
            children: <Widget>[
              TextField(
                controller: creditCardNumber,
                autofillHints: const <String>[AutofillHints.creditCardNumber],
              ),
              TextField(
                controller: creditCardSecurityCode,
                autofillHints: const <String>[
                  AutofillHints.creditCardSecurityCode,
                ],
              ),
            ],
          ),
        ),
        const Text('Contact Phone Number'),
        // The phone number field can still be autofilled despite lacking an
        // `AutofillScope`.
        TextField(
          controller: phoneNumber,
          autofillHints: const <String>[AutofillHints.telephoneNumber],
        ),
      ],
    );
  }
}
